TEMPLATE     = app
HEADERS	     = SimpleAsker.h \
    ExtQPushButton.h
SOURCES	     = SimpleAsker.cpp main.cpp \
    ExtQPushButton.cpp
QT          += widgets multimedia
RESOURCES   += resources/commonRes.qrc
QMAKE_RESOURCE_FLAGS += -no-compress
windows:TARGET = ../SimpleAsker
windows:RC_FILE = resources/common/icon.rc

DISTFILES += \
    android/AndroidManifest.xml \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}

