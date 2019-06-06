var USBMouseMessage = function(btnStatus, x, y, wheelrotation) {
    this.m_btnStatus = btnStatus || 0;
    this.m_xDisp = x || 0;
    this.m_yDisp = y || 0;
    this.m_wheelrotation = wheelrotation || 0;

    this.m_mousemode = this.INVALID_MOUSE_MODE;
    this.m_btnStatus = 0;

    this.SeqNum = 0;
};

USBMouseMessage.prototype = _.extend({
    DIRABS_MAX_SCALED_X: 32767,
    DIRABS_MAX_SCALED_Y: 32767,

    INVALID_MOUSE_MODE: -1,
    RELATIVE_MOUSE_MODE: 1,
    ABSOLUTE_MOUSE_MODE: 2,
    OTHER_MOUSE_MODE: 3,

    set: function(btnStatus, x, y, mode, width, height, wheelrotation) {
        this.m_btnStatus = btnStatus;
        this.m_xDisp = x;
        this.m_yDisp = y;
        this.m_mousemode = mode;
        this.m_screenwidth = width;
        this.m_screenheight = height;
        this.m_wheelrotation = wheelrotation;
    },

    ABSReport: function() {
        var x = (this.m_xDisp * this.DIRABS_MAX_SCALED_X) / this.m_screenwidth + 0.5,
            y = (this.m_yDisp * this.DIRABS_MAX_SCALED_Y) / this.m_screenheight + 0.5;

        this.m_report = new DataStream(IVTP.HDR_SIZE + this.IUSB_HID_HDR_SIZE - 1 + this.USB_PKT_MOUSE_ABS_REP_SIZE);


        var DataPktLen = this.IUSB_HID_HDR_SIZE - 1 + this.USB_PKT_MOUSE_ABS_REP_SIZE - this.IUSB_HDR_SIZE;

        this.m_report.seek(0);
        //ivtp_hdr
        this.m_report.writeUint16(IVTP.CMD_SEND_HID_PACKET, DataStream.LITTLE_ENDIAN);
        this.m_report.writeUint32(this.IUSB_HID_HDR_SIZE - 1 + this.USB_PKT_MOUSE_ABS_REP_SIZE, DataStream.LITTLE_ENDIAN);
        this.m_report.writeUint16(0, DataStream.LITTLE_ENDIAN);
        //usb header
        this.m_report.writeString('IUSB    ');
        this.m_report.writeInt8(this.IUSB_MAJOR_NUM);
        this.m_report.writeInt8(this.IUSB_MINOR_NUM);
        this.m_report.writeInt8(this.IUSB_HDR_SIZE);
        this.m_report.writeInt8(0); // checksum to be filled below
        this.m_report.writeInt32(DataPktLen);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(this.IUSB_DEVICE_MOUSE);
        this.m_report.writeInt8(this.IUSB_PROTO_MOUSE_DATA);
        this.m_report.writeInt8(this.IUSB_FROM_REMOTE & 0xff);
        this.m_report.writeInt8(this.IUSB_MOUSE_DEVNUM);
        this.m_report.writeInt8(this.IUSB_MOUSE_IFNUM);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(0);
        this.m_report.writeInt32(this.SeqNum);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(this.USB_PKT_MOUSE_ABS_REP_SIZE & 0xff);

        this.m_report.writeInt8(this.m_btnStatus);
        this.m_report.writeInt16(x, DataStream.LITTLE_ENDIAN);
        this.m_report.writeInt16(y, DataStream.LITTLE_ENDIAN);
        this.m_report.writeInt8(this.m_wheelrotation);

        var temp = 0;
        for (var i = IVTP.HDR_SIZE; i < this.IUSB_HDR_SIZE + IVTP.HDR_SIZE; i++) {
            this.m_report.seek(i);
            temp = (temp + (this.m_report.readInt8() & 0xff)) & 0xff;
        }
        this.m_report.seek(IVTP.HDR_SIZE + 11);
        this.m_report.writeInt8(-(temp & 0xff));
        this.SeqNum++;
        this.m_report.seek(0);
        return this.m_report;
    },

    RELReport: function() {
        this.m_report = new DataStream(IVTP.HDR_SIZE + this.IUSB_HID_HDR_SIZE - 1 + this.USB_PKT_MOUSE_REL_REP_SIZE);

        var cur_x_disp = (this.last_x != null ? this.m_xDisp - this.last_x : 0);
        var cur_y_disp = (this.last_y != null ? this.m_yDisp - this.last_y : 0);

        this.last_x = this.m_xDisp;
        this.last_y = this.m_yDisp;

        var DataPktLen = this.IUSB_HID_HDR_SIZE - 1 + this.USB_PKT_MOUSE_REL_REP_SIZE - this.IUSB_HDR_SIZE;

        this.m_report.seek(0);
        //ivtp_hdr
        this.m_report.writeUint16(IVTP.CMD_SEND_HID_PACKET, DataStream.LITTLE_ENDIAN);
        this.m_report.writeUint32(this.IUSB_HID_HDR_SIZE - 1 + this.USB_PKT_MOUSE_REL_REP_SIZE, DataStream.LITTLE_ENDIAN);
        this.m_report.writeUint16(0, DataStream.LITTLE_ENDIAN);
        //usb header
        this.m_report.writeString('IUSB    ');
        this.m_report.writeInt8(this.IUSB_MAJOR_NUM);
        this.m_report.writeInt8(this.IUSB_MINOR_NUM);
        this.m_report.writeInt8(this.IUSB_HDR_SIZE);
        this.m_report.writeInt8(0); // checksum to be filled below
        this.m_report.writeInt32(DataPktLen);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(this.IUSB_DEVICE_MOUSE);
        this.m_report.writeInt8(this.IUSB_PROTO_MOUSE_DATA);
        this.m_report.writeInt8(this.IUSB_FROM_REMOTE & 0xff);
        this.m_report.writeInt8(this.IUSB_MOUSE_DEVNUM);
        this.m_report.writeInt8(this.IUSB_MOUSE_IFNUM);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(0);
        this.m_report.writeInt32(this.SeqNum);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(this.USB_PKT_MOUSE_ABS_REP_SIZE & 0xff);

        this.m_report.writeInt8(this.m_btnStatus);
        this.m_report.writeInt8(cur_x_disp);
        this.m_report.writeInt8(cur_y_disp);
        this.m_report.writeInt8(this.m_wheelrotation);

        //add checksum
        var temp = 0;
        for (var i = IVTP.HDR_SIZE; i < this.IUSB_HDR_SIZE + IVTP.HDR_SIZE; i++) {
            this.m_report.seek(i);
            temp = (temp + (this.m_report.readInt8() & 0xff)) & 0xff;
        }
        this.m_report.seek(IVTP.HDR_SIZE + 11);
        this.m_report.writeInt8(-(temp & 0xff));
        this.SeqNum++;
        this.m_report.seek(0);
        return this.m_report;
    },

    report: function() {
        if (this.m_mousemode == this.ABSOLUTE_MOUSE_MODE) {
            return this.ABSReport();
        } else {
            return this.RELReport();
        }
    }
}, USBMessage);
if (typeof define == "function" && define.amd) {
    define(function() {
        return USBMouseMessage;

    });
}