# - Language files -
SET(QMFILES_PATH "${st-base_SOURCE_DIR}/translations")
FILE(GLOB qmfiles "${QMFILES_PATH}/st_*.qm")
INSTALL(FILES ${qmfiles} DESTINATION ${ST_LOCALES_OUTPUT_PATH})			

