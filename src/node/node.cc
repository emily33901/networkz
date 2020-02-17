#include "node.hh"

#include "common/toml.hh"

#include "console/console.hh"
#include "filesystem/filesystem.hh"

namespace node {

Node::Node() {
}

Node::~Node() {
}

Node *Node::Clone() {
    // This is a low level clone that needs to be wrapped by the workspace or similar

    auto newNode = factory::Create(name);
    auto n       = *newNode;

    n->name       = name;
    n->desc       = desc;
    n->file       = file;
    n->inherited  = inherited;
    n->attributes = attributes;

    return n;
}

void Node::Frame() {
}

void Node::Think() {
}

namespace factory {
std::unordered_map<std::string, Node *> nodeTemplates;

Node *baseTemplate = nullptr;

Node *InheritInternal(Node *n, Node *base) {
    // Inherit attributes
    for (const auto &attr : base->attributes) {
        if (std::find_if(n->attributes.begin(), n->attributes.end(),
                         [&attr](const auto &a) { return a.key == attr.key; }) == n->attributes.end()) {
            n->attributes.push_back(attr);
        }
    }

    n->inherited.push_back(base->name);

    // TODO pins (do we even want to inherit them?)

    // TODO check for overlaps (although shouldnt be an issue)
    // TODO in order to make sure there arent any problems we probably need our own pin mapping here
    // so that the pins are kept unique
    n->inputs.insert(n->inputs.end(), base->inputs.begin(), base->inputs.end());
    n->outputs.insert(n->outputs.end(), base->outputs.begin(), base->inputs.end());

    return n;
}

Option<Node *> Inherit(Node *n, const std::string &name) {
    auto v = nodeTemplates.find(name);

    if (v != nodeTemplates.end()) return Some(InheritInternal(n, v->second));

    return None();
}

const std::string &ApplyTemplate(Node *n, const std::string &text) {
    const auto table = cpptoml::parse_text(text);

    const auto name     = table->template get_qualified_as<std::string>("file.name");
    const auto desc     = table->template get_qualified_as<std::string>("file.desc");
    const auto inherits = table->template get_array_of<std::string>("file.inherits").value_or(std::initializer_list<std::string>{});

    n->name = *name;
    n->desc = *desc;

    n->inherited = inherits;

    for (const auto &x : inherits) Inherit(n, x);

    // TODO pins & attrs

    const auto attrs = table->get_table("attrs");

    for (const auto &x : *attrs) {
        console::Log("[attr] Adding %s to %s", x.first.c_str(), n->name.c_str());
        n->attributes.push_back(Attribute{x.first, x.second->as<std::string>()->get()});
    }

    const auto inputs = table->get_table("inputs");

    for (const auto &x : *inputs) {
        console::Log("[inputs] Adding %s to %s", x.first.c_str(), n->name.c_str());
        n->inputs.push_back(Pin{x.first, x.second->as<std::string>()->get()});
    }

    for (const auto &x : *table->get_table("outputs")) {
        console::Log("[outputs] Adding %s to %s", x.first.c_str(), n->name.c_str());
        n->outputs.push_back(Pin{x.first, x.second->as<std::string>()->get()});
    }

    return *name;
} // namespace factory

void FetchNodeTemplate(const std::string &f) {
    filesystem::Request("nodes/" + f, [](auto path, auto text) {
        if (text == None()) {
            printf("Unable to load node %s\n", path.c_str());
            return;
        }
        auto newNode  = new Node();
        newNode->file = path;

        // If we have already loaded the base template then inherit all of them over

        auto name = ApplyTemplate(newNode, *text);

        if (baseTemplate != nullptr) {
            InheritInternal(newNode, baseTemplate);
        }

        if (path == "nodes/base.toml") {
            // Base is a special case
            baseTemplate = newNode;

            // All other templates up to this point now need to inherit baseTemplate

            for (auto &[k, v] : nodeTemplates) {
                if (std::find(v->inherited.begin(), v->inherited.end(), "base") == v->inherited.end()) continue;
                v->inherited.push_back("base");
                InheritInternal(v, baseTemplate);
            }
        } else {
            nodeTemplates[name] = newNode;
        }
    });
}

bool Init() {
    // Get the node file
    filesystem::Request("nodes/files.toml", [](auto path, auto text) {
        if (text == None()) {
            printf("Unable to get node list!\n");
        } else {
            auto table = cpptoml::parse_text(*text);

            auto files = table->template get_array_of<std::string>("files");

            for (const auto &f : *files) {
                // Now try and fetch all of these files
                FetchNodeTemplate(f);
            }
        }
    });

    console::Register("node/templates", [](auto args) {
        for (const auto &n : nodeTemplates) {
            console::Log("%s - %s", n.first.c_str(), n.second->file.c_str());
        }
    });

    console::Register("node/describe", [](auto args) {
        auto it = nodeTemplates.find(args);
        if (it == nodeTemplates.end()) {
            console::Log("[error] Unable to find %s", args.c_str());
            return;
        }

        auto node = it->second;

        console::Log("Node %s (%s)", node->name.c_str(), node->file.c_str());
        console::Log("- attrs");
        for (const auto &[k, v] : node->attributes) {
            console::Log("\t%s:%s", k.c_str(), v.c_str());
        }
        console::Log("- inherits");
        for (const auto &v : node->inherited) {
            console::Log("\t%s", v.c_str());
        }
        console::Log("- inputs");
        for (const auto &v : node->inputs) {
            console::Log("\t%s:%s", v.name.c_str(), v.type.c_str());
        }
        console::Log("- outputs");
        for (const auto &v : node->outputs) {
            console::Log("\t%s:%s", v.name.c_str(), v.type.c_str());
        }
    });

    console::Register("node/describe-all", [](auto args) {
        for (const auto &[k, v] : nodeTemplates) {
            console::Execute("node/describe " + k);
        }
    });

    return true;
}

Option<Node *> Create(const std::string &name) {
    auto v = nodeTemplates.find(name);

    if (v != nodeTemplates.end()) return Some(v->second->Clone());

    return None();
}

void Destroy(Node *n) {
    delete n;
}

} // namespace factory
} // namespace node