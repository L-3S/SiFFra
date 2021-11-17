$$(FBSF_HOME){
    FBSF_HOME=$$(FBSF_HOME)
} else {
    FBSF_HOME=$$PWD
}
FBSF_LIB_DIR_ROOT=$$PWD/lib/
FBSF_FRAMEWORK_DIR=$$PWD/FbsfFramework/
FBSF_PLUGINS_DIR=$$FBSF_FRAMEWORK_DIR/fbsfplugins

CONFIG(debug, debug|release) {
    DESTDIR = $$FBSF_LIB_DIR_ROOT/debug
} else {
    DESTDIR = $$FBSF_LIB_DIR_ROOT/release
}
BATCH {
    DESTDIR = $$FBSF_LIB_DIR_ROOT/batch
}
message( "destdir" $$DESTDIR )

FBSF_LIB_DIR =$$DESTDIR
