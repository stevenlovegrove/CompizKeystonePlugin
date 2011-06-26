find_package (Compiz REQUIRED)

include (CompizPlugin)

set(CMAKE_CXX_FLAGS "-g -Wall")

compiz_plugin (keystone PLUGINDEPS composite opengl mousepoll PKGDEPS cairo cairo-xlib-xrender)
