import shutil
import os

import sys

print("copying %s to %s" % (sys.argv[1], sys.argv[2]))

if os.path.isdir(sys.argv[2]):
    shutil.rmtree(sys.argv[2])

shutil.copytree(sys.argv[1], sys.argv[2])
