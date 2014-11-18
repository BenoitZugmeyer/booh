{
    "targets": [
        {
            "target_name": "booh",
            "sources": [
                "src/booh.cc",
                "src/browser.cc",

                "src/webpage.cc",
                "src/webpage.h",
                "<(INTERMEDIATE_DIR)/moc_webpage.cc",

                "src/bridge.cc",
                "src/bridge.h",
                "<(INTERMEDIATE_DIR)/moc_bridge.cc",
            ],
            "rules": [
                {
                    "rule_name": "moc_headers",
                    "extension": "h",
                    "outputs": [
                        "<(INTERMEDIATE_DIR)/moc_<(RULE_INPUT_ROOT).cc"
                    ],
                    "action": [
                        "moc", "<(_cflags)", "<(RULE_INPUT_PATH)",
                        "-o", "<(INTERMEDIATE_DIR)/moc_<(RULE_INPUT_ROOT).cc"
                    ]
                }
            ],
            "qt_modules": "Qt5Core Qt5Gui Qt5Test Qt5Widgets Qt5WebKitWidgets",
            "cflags": [
                "<!@(pkg-config --cflags <(_qt_modules))",
                "-std=c++11"
            ],
            "ldflags": [
                "<!@(pkg-config --libs-only-L --libs-only-other <(_qt_modules))"
            ],
            "libraries": [
                "<!@(pkg-config --libs-only-l <(_qt_modules))"
            ]
        }
    ]
}
