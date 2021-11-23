$$(FBSF_HOME){
    FBSF_HOME=$$(FBSF_HOME)
} else {
    FBSF_HOME=$$PWD
}
FBSF_LIB_DIR_ROOT=$$PWD/lib/
FBSF_FRAMEWORK_DIR=$$PWD/FbsfFramework/
FBSF_PLUGINS_DIR=$$FBSF_FRAMEWORK_DIR/fbsfplugins

CONFIG(debug, debug|release) {
    BUILD_MODE = debug
} else {
    BUILD_MODE = release
}
BATCH {
    BUILD_MODE = batch
}
DESTDIR = $$FBSF_LIB_DIR_ROOT/$$BUILD_MODE
message( "destdir" $$DESTDIR )

FBSF_LIB_DIR =$$DESTDIR
