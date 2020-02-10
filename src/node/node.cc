#include "node.hh"

#include "common/toml.hh"

#include "filesystem/filesystem.hh"

namespace node {

namespace factory {
std::unordered_map<std::string, Node *> nodeTemplates;

Node *baseTemplate = nullptr;

const std::string &ApplyTemplate(Node *n, const std::string &text) {
    const auto table = cpptoml::parse_text(text);

    const auto name     = table->get_qualified_as<std::string>("file.name");
    const auto desc     = table->get_qualified_as<std::string>("file.desc");
    const auto inherits = table->get_array_of<std::string>("file.inherits").value_or(std::initializer_list<std::string>{});

    n->name = *name;
    n->desc = *desc;

    n->inherited = inherits;

    for (const auto &x : inherits) Inherit(n, x);

    // TODO pins & attrs
}

void FetchNodeTemplate(const std::string &f) {
    filesystem::Request("nodes/" + f, [](auto path, auto text) {
        if (text == None()) {
            printf("Unable to load node %s\n", path);
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
            nodeTemplates[name];
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

            auto files = table->get_array_of<std::string>("files");

            for (const auto &f : *files) {
                // Now try and fetch all of these files
                FetchNodeTemplate(f);
            }
        }
    });
}

Option<Node *> Create(const std::string &name) {
    auto v = nodeTemplates.find(name);

    if (v != nodeTemplates.end()) return Some(v->second->Clone());

    return None();
}

void Destroy(Node *n) {
    delete n;
}

Node *InheritInternal(Node *n, Node *base) {
    // TODO figure out how this works
}

Option<Node *> Inherit(Node *n, const std::string &name) {
    auto v = nodeTemplates.find(name);

    if (v != nodeTemplates.end()) return Some(InheritInternal(n, v->second));

    return None();
}

} // namespace factory
} // namespace node