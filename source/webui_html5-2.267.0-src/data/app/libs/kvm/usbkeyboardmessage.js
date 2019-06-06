var USBKeyBoardMessage = function() {
    this.m_devType = this.IUSB_DEVICE_KEYBD;
    this.m_protocol = this.IUSB_PROTO_KEYBD_DATA;
    this.m_datalen = this.USB_PKT_KEYBDREP_SIZE;
    this.m_report = new DataStream(IVTP.HDR_SIZE + this.IUSB_HID_HDR_SIZE - 1 + this.USB_PKT_KEYBDREP_SIZE);
};

USBKeyBoardMessage.prototype = _.extend({

    SeqNum: 0,

    /*init	:	function() {
		var ivtp = new DataStream();
		ivtp.writeStruct(['type', 'uint16le',
			'pkt_size', 'uint32le',
			'status', 'uint16le'],
			{
				'type' : 0x01,
				'pkt_size' : this.IUSB_HID_HDR_SIZE - 1 + this.USB_PKT_KEYBDREP_SIZE,
				'status' : 0
			});
		this.ivtp_hdr = ivtp.buffer;
		this.m_devType = this.IUSB_DEVICE_KEYBD;
		this.m_protocol = this.IUSB_PROTO_KEYBD_DATA;
		this.m_datalen = this.USB_PKT_KEYBDREP_SIZE;
		this.m_report = new DataStream(); //(IVTP.HDR_SIZE+this.IUSB_HID_HDR_SIZE-1+this.USB_PKT_KEYBDREP_SIZE);
	},*/

    set: function(keyCode, keyLocation, keyPress) {
        this.keyCode = keyCode;
        this.keyLocation = keyLocation;
        this.keyPress = keyPress;
    },

    report: function() {
        //console.log("kbd report");
        var DataPktLen = this.IUSB_HID_HDR_SIZE - 1 + this.USB_PKT_KEYBDREP_SIZE - this.IUSB_HDR_SIZE;

        //ivtp_header packet
        /*this.m_report.writeStruct(['type', 'uint16le',
			'pkt_size', 'uint32le',
			'status', 'uint16le'],
			{
				'type' : 0x01,
				'pkt_size' : this.IUSB_HID_HDR_SIZE - 1 + this.USB_PKT_KEYBDREP_SIZE,
				'status' : 0
			});*/
        //ivtp_header packet
        this.m_report.seek(0);
        this.m_report.writeUint16(IVTP.CMD_SEND_HID_PACKET, DataStream.LITTLE_ENDIAN);
        this.m_report.writeUint32(this.IUSB_HID_HDR_SIZE - 1 + this.USB_PKT_KEYBDREP_SIZE, DataStream.LITTLE_ENDIAN);
        this.m_report.writeUint16(0, DataStream.LITTLE_ENDIAN);
        //console.log("pkt_size",this.IUSB_HID_HDR_SIZE - 1 + this.USB_PKT_KEYBDREP_SIZE);
        //usb header
        //this.m_report.writeInt8Array(iusb_signature);
        this.m_report.writeString('IUSB    ');
        this.m_report.writeInt8(this.IUSB_MAJOR_NUM);
        this.m_report.writeInt8(this.IUSB_MINOR_NUM);
        this.m_report.writeInt8(this.IUSB_HDR_SIZE);
        this.m_report.writeInt8(0); //checksum - init to 0
        this.m_report.writeInt32(DataPktLen, DataStream.LITTLE_ENDIAN);
        //console.log("DataPktLen",DataPktLen);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(this.IUSB_DEVICE_KEYBD);
        this.m_report.writeInt8(this.IUSB_PROTO_KEYBD_DATA);
        this.m_report.writeInt8(this.IUSB_FROM_REMOTE & 0xff);
        this.m_report.writeInt8(this.IUSB_KEYBD_DEVNUM);
        this.m_report.writeInt8(this.IUSB_KEYDB_IFNUM);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(0);
        this.m_report.writeInt32(this.SeqNum, DataStream.LITTLE_ENDIAN);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(0);
        this.m_report.writeInt8(0);

        this.m_report.writeInt8(this.USB_PKT_KEYBDREP_SIZE & 0xff);

		if(this.m_USBKeyProcessor === null) {
			this.m_USBKeyProcessor = new USBKeyBoardEnglish();
		}

        var keybd_report_data = this.m_USBKeyProcessor.convertKeyCode(this.keyCode, this.keyLocation, this.keyPress);
        //console.log(keybd_report_data);
		if(keybd_report_data === null) {
            this.m_report.writeUint8Array(new Uint8Array(6));
            this.KeyBoardDataNull = true;
        } else {
            this.m_report.writeInt8Array(keybd_report_data.readInt8Array());
        }

        //Calculate modulo100 checksum
        var temp = 0;
        for (var i = IVTP.HDR_SIZE; i < this.IUSB_HDR_SIZE + IVTP.HDR_SIZE; i++) {
            this.m_report.seek(i);
            temp = (temp + (this.m_report.readInt8() & 0xff)) & 0xff;
        }

        this.m_report.seek(IVTP.HDR_SIZE + 11);
        //console.log((temp & 0xff), -(temp & 0xff));
        this.m_report.writeInt8(-(temp & 0xff));
        this.m_report.seek(this.m_report.byteLength);
        this.SeqNum++;
        this.m_report.seek(0);
        //		console.log(this.m_report.readInt8Array());
        return this.m_report;
    },

    setAutoKeybreakMode: function(state) {
        this.m_USBKeyProcessor.setAutoKeybreakMode(state);
    },

    getAutoKeybreakMode: function() {
        return this.m_USBKeyProcessor.getAutoKeybreakMode();
    },

    getUSBKeyProcessor: function() {
        return this.m_USBKeyProcessor;
    },

    setUSBKeyProcessor: function(keyProcessor) {
        this.m_USBKeyProcessor = keyProcessor;
    }

}, USBMessage);
if (typeof define == "function" && define.amd) {
    define(function() {
        return USBKeyBoardMessage;
    });
}