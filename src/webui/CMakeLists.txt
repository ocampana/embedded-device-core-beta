set(LOCALE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/locales")
set(OUTPUT_BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}/locales")

file(GLOB_RECURSE PO_FILES "${LOCALE_DIR}/*.po")

set(MO_FILES "")

foreach(PO_FILE ${PO_FILES})
    # Get the relative path to preserve the directory structure (e.g., it/LC_MESSAGES/base.po)
    file(RELATIVE_PATH REL_PO "${LOCALE_DIR}" "${PO_FILE}")
    string(REPLACE ".po" ".mo" REL_MO "${REL_PO}")
    
    set(MO_FILE "${OUTPUT_BASE_DIR}/${REL_MO}")
    get_filename_component(MO_DIR "${MO_FILE}" DIRECTORY)

    # Create the directory for the .mo file
    add_custom_command(
        OUTPUT "${MO_FILE}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${MO_DIR}"
        COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o "${MO_FILE}" "${PO_FILE}"
        DEPENDS "${PO_FILE}"
        COMMENT "Compiling translation ${REL_PO} to ${REL_MO}"
    )
    
    list(APPEND MO_FILES "${MO_FILE}")
endforeach()

# 1. Update/Generate the .pot template (Extracts strings from source)
# Run with: make update-pot
add_custom_target(update-pot
    COMMAND pygettext3 -o ${LOCALE_DIR}/base.pot ${CMAKE_CURRENT_SOURCE_DIR}/src/webui/*.py ${CMAKE_CURRENT_SOURCE_DIR}/templates/*.html
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Extracting strings to base.pot"
)

# 2. Update existing .po files with new strings from the .pot
# Run with: make update-po
add_custom_target(update-po
    DEPENDS update-pot
    # Use '\\;' to ensure the semicolon reaches the shell
    COMMAND find ${LOCALE_DIR} -name "*.po" -exec msgmerge -U {} ${LOCALE_DIR}/base.pot \\;
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Merging new strings into existing .po files"
    VERBATIM
)

# 3. Create a brand new language from the template
# Usage: make add-lang LANG=fr
add_custom_target(add-lang
    # This ensures update-pot runs automatically if base.pot is missing
    DEPENDS update-pot 
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LOCALE_DIR}/$ENV{LANG}/LC_MESSAGES
    COMMAND msginit --no-translator -l $ENV{LANG} -i ${LOCALE_DIR}/base.pot -o ${LOCALE_DIR}/$ENV{LANG}/LC_MESSAGES/base.po
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Initializing new language: $ENV{LANG}"
    VERBATIM
)

add_custom_target(translations ALL DEPENDS ${MO_FILES})


add_custom_command (
	OUTPUT Core/CreateInitialCredentialPayload.py Core/ErrorPayload.py Core/__init__.py Core/Message.py Core/MessageType.py Core/OkPayload.py Core/Payload.py
	COMMAND ${FLATC} -p ../ipc_protocol.fbs
	VERBATIM)

#install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/" DESTINATION bin/webserver)
## Install the compiled translations from the build directory
#install(DIRECTORY "${OUTPUT_BASE_DIR}/" DESTINATION bin/webserver/locales)
