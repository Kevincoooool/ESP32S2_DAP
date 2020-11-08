
set(COMPONENT_REQUIRES ". ${PROJECT_DIR}/components/CMSIS-DAP")
set(COMPONENT_ADD_INCLUDEDIRS ". ${PROJECT_DIR}/components/CMSIS-DAP/Include")
idf_component_register(SRCS 
"main.c" 
"hid_task.c"
"webusb_task.c"
"cdc_task.c"
"msc_task.c" 
INCLUDE_DIRS . ${COMPONENT_DIR} . ${PROJECT_DIR}/components/CMSIS-DAP/Include)