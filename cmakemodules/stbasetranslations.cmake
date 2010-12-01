
#update_ts target
# to update ts files run: make update_ts
configure_file(${st-base_SOURCE_DIR}/cmakemodules/lupdate.sh.in ${CMAKE_CURRENT_SOURCE_DIR}/translations/lupdate.sh)
add_custom_target (update_ts ${CMAKE_CURRENT_SOURCE_DIR}/translations/lupdate.sh )

