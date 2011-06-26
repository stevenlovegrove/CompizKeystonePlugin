find_package (Compiz REQUIRED)

include (CompizPlugin)

compiz_plugin (keystone PLUGINDEPS composite opengl mousepoll PKGDEPS cairo cairo-xlib-xrender)
