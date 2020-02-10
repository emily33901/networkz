from shutil import copytree

import sys

print("copying %s to %s" % (sys.argv[1], sys.argv[2]))

copytree(sys.argv[1], sys.argv[2])
