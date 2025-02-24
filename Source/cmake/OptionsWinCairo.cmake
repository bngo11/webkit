set(WTF_PLATFORM_WIN_CAIRO 1)

include(OptionsWin)

# TODO: Move this above OptionsWin when WebKitLegacy is deprecated
set(ENABLE_WEBKIT ON)

find_package(Cairo 1.15.12 REQUIRED)
find_package(CURL 7.60.0 REQUIRED)
find_package(ICU 60.2 REQUIRED COMPONENTS data i18n uc)
find_package(JPEG 1.5.2 REQUIRED)
find_package(LibXml2 2.9.7 REQUIRED)
find_package(OpenSSL 2.0.0 REQUIRED)
find_package(PNG 1.6.34 REQUIRED)
find_package(SQLite3 3.23.1 REQUIRED)
find_package(ZLIB 1.2.11 REQUIRED)
find_package(LibPSL 0.20.2 REQUIRED)

if (ENABLE_XSLT)
    find_package(LibXslt 1.1.32 REQUIRED)
endif ()

# Optional packages
find_package(OpenJPEG 2.3.1)
if (OpenJPEG_FOUND)
    SET_AND_EXPOSE_TO_BUILD(USE_OPENJPEG ON)
endif ()

find_package(WebP COMPONENTS demux)
if (WebP_FOUND)
    SET_AND_EXPOSE_TO_BUILD(USE_WEBP ON)
endif ()

# TODO: Add a check for HAVE_RSA_PSS for support of CryptoAlgorithmRSA_PSS
# https://bugs.webkit.org/show_bug.cgi?id=206635

set(USE_ANGLE_EGL ON)

SET_AND_EXPOSE_TO_BUILD(USE_CAIRO ON)
SET_AND_EXPOSE_TO_BUILD(USE_CF ON)
SET_AND_EXPOSE_TO_BUILD(USE_CURL ON)
SET_AND_EXPOSE_TO_BUILD(USE_GRAPHICS_LAYER_TEXTURE_MAPPER ON)
SET_AND_EXPOSE_TO_BUILD(USE_EGL ON)
SET_AND_EXPOSE_TO_BUILD(USE_OPENGL_ES ON)
SET_AND_EXPOSE_TO_BUILD(HAVE_OPENGL_ES_3 ON)
SET_AND_EXPOSE_TO_BUILD(USE_OPENSSL ON)
SET_AND_EXPOSE_TO_BUILD(USE_TEXTURE_MAPPER ON)
SET_AND_EXPOSE_TO_BUILD(USE_TEXTURE_MAPPER_GL ON)
SET_AND_EXPOSE_TO_BUILD(USE_MEDIA_FOUNDATION ON)
SET_AND_EXPOSE_TO_BUILD(USE_INSPECTOR_SOCKET_SERVER ${ENABLE_REMOTE_INSPECTOR})

SET_AND_EXPOSE_TO_BUILD(ENABLE_GRAPHICS_CONTEXT_GL ON)
SET_AND_EXPOSE_TO_BUILD(ENABLE_DEVELOPER_MODE ${DEVELOPER_MODE})

set(COREFOUNDATION_LIBRARY CFlite)

add_definitions(-DWTF_PLATFORM_WIN_CAIRO=1)
add_definitions(-DNOCRYPT)

# Override headers directories
set(ANGLE_FRAMEWORK_HEADERS_DIR ${CMAKE_BINARY_DIR}/ANGLE/Headers)
set(WTF_FRAMEWORK_HEADERS_DIR ${CMAKE_BINARY_DIR}/WTF/Headers)
set(JavaScriptCore_FRAMEWORK_HEADERS_DIR ${CMAKE_BINARY_DIR}/JavaScriptCore/Headers)
set(JavaScriptCore_PRIVATE_FRAMEWORK_HEADERS_DIR ${CMAKE_BINARY_DIR}/JavaScriptCore/PrivateHeaders)
set(PAL_FRAMEWORK_HEADERS_DIR ${CMAKE_BINARY_DIR}/PAL/Headers)
set(WebCore_PRIVATE_FRAMEWORK_HEADERS_DIR ${CMAKE_BINARY_DIR}/WebCore/PrivateHeaders)
set(WebKitLegacy_FRAMEWORK_HEADERS_DIR ${CMAKE_BINARY_DIR}/WebKitLegacy/Headers)
set(WebKit_FRAMEWORK_HEADERS_DIR ${CMAKE_BINARY_DIR}/WebKit/Headers)
set(WebKit_PRIVATE_FRAMEWORK_HEADERS_DIR ${CMAKE_BINARY_DIR}/WebKit/PrivateHeaders)

# Override derived sources directories
set(WTF_DERIVED_SOURCES_DIR ${CMAKE_BINARY_DIR}/WTF/DerivedSources)
set(JavaScriptCore_DERIVED_SOURCES_DIR ${CMAKE_BINARY_DIR}/JavaScriptCore/DerivedSources)
set(WebCore_DERIVED_SOURCES_DIR ${CMAKE_BINARY_DIR}/WebCore/DerivedSources)
set(WebKitLegacy_DERIVED_SOURCES_DIR ${CMAKE_BINARY_DIR}/WebKitLegacy/DerivedSources)
set(WebKit_DERIVED_SOURCES_DIR ${CMAKE_BINARY_DIR}/WebKit/DerivedSources)

# Override scripts directories
set(WTF_SCRIPTS_DIR ${CMAKE_BINARY_DIR}/WTF/Scripts)
set(JavaScriptCore_SCRIPTS_DIR ${CMAKE_BINARY_DIR}/JavaScriptCore/Scripts)

# Override library types
set(WebCore_LIBRARY_TYPE OBJECT)
set(WebCoreTestSupport_LIBRARY_TYPE OBJECT)
