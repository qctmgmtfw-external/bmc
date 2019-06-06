var USBMessage = {
    IUSB_HID_HDR_SIZE: 34,
    USB_PKT_KEYBDREP_SIZE: 8,
    USB_PKT_MOUSE_REL_REP_SIZE: 4,
    USB_PKT_MOUSE_ABS_REP_SIZE: 6,
    IUSB_HDR_SIZE: 32,
    IUSB_PROTO_KEYBD_DATA: 0x10,
    IUSB_PROTO_MOUSE_DATA: 0x20,
    IUSB_DEVICE_KEYBD: 0x30,
    IUSB_DEVICE_MOUSE: 0x31,
    IUSB_MAJOR_NUM: 0x01,
    IUSB_MINOR_NUM: 0x00,
    IUSB_KEYBD_DEVNUM: 0x02,
    IUSB_KEYDB_IFNUM: 0x00,
    IUSB_MOUSE_DEVNUM: 0x02,
    IUSB_MOUSE_IFNUM: 0x01,
    IUSB_FROM_REMOTE: 0x80,
    IUSB_TO_REMOTE: 0x00,
    m_devType: null,
    m_protocol: null,
    m_datalen: null,
    m_reserved: new Int8Array(2)
};
if (typeof define == "function" && define.amd) {
    define(function() {
        return USBMessage;
    });
}