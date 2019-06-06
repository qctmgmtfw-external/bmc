var IUSBSCSI = function(buffer, preBuffered) {

    this.LBA = null; //eject 0x00020000 and load 0x00030000

    if (buffer instanceof IUSBHeader) {
        this.header = buffer;
        this.instance = this.header.instance;
        this.dataLength = this.header.dataPacketLength;

    } else if (buffer instanceof DataStream) {
        this.preBuffered = preBuffered;
        this.header = new IUSBHeader();
        this.header.read(buffer);
        this.header.dataPacketLength = buffer.byteLength - IUSBHeader.HEADER_LEN; //this.header.headerLength
        //this.header.dataPacketLength = buffer.byteLength - this.header.headerLength;
        //console.log("iusb scsi lengths ... ", buffer.byteLength, this.header.headerLength, this.header.dataPacketLength);
        this.dataLength = this.header.dataPacketLength;
        //type Uint8Array
        this.data = new DataStream(buffer.readUint8Array().buffer); //buffer.byteLength - buffer.position
    }

};

IUSBSCSI.IUSB_SCSI_PKT_SIZE = 62;
IUSBSCSI.IUSB_SCSI_PKT_SIZE_WITHOUT_HEADER = 30;
IUSBSCSI.OPCODE_EJECT = 0x1b;
IUSBSCSI.OPCODE_KILL_REDIR = 0xf6;
IUSBSCSI.DIRECTION_FROM_CLIENT = 0x80;
IUSBSCSI.LBA_EJECTED = 0x00020000;
IUSBSCSI.LBA_LOADED = 0x00030000;

IUSBSCSI.prototype = {

    writePacket: function(buffer) {
        /* Reverse the direction to FROM_REMOTE */
        this.header.direction = IUSBSCSI.DIRECTION_FROM_CLIENT;
        this.header.write(buffer);


        if (!this.preBuffered) {
            this.data.seek(0);
            buffer.writeUint8Array(this.data.readUint8Array());
        }
        //buffer.writeUint8Array(new Uint8Array(IUSBHeader.HEADER_LEN)); //for testing
    },

    readData: function(buffer) {
        this.data = new DataStream(this.dataLength);

        if (buffer.byteLength - buffer.position < this.dataLength) {
            //console.log("buffer low",buffer.byteLength - buffer.position, this.dataLength);
            throw new Error("ISCSI data buffer under flow");
        }

        buffer.seek(IUSBHeader.HEADER_LEN);

        this.data.writeUint8Array(buffer.readUint8Array()); //this.dataLength-IUSBSCSI.IUSB_SCSI_PKT_SIZE

        this.data.seek(0);
        //console.log("iusbscsi data ",this.data.readUint8Array());

        this.data.seek(9);
        this.opcode = this.data.readUint8() & 0xff;

        this.data.seek(11);
        this.LBA = this.data.readUint32();


        if (this.opcode == CDROM.DEVICE_REDIRECTION_ACK && this.dataLength > 30) {
            this.data.seek(30);
            this.connectionStatus = this.data.readUint8();
            try {
                this.otherIP = this.data.readString(24);
            } catch (e) {
                this.otherIP = "";
                throw new Error("ISCSI otherIP buffer under flow");
            }

        }
    }

};

if (typeof define == "function" && define.amd) {
    define(function() {
        return IUSBSCSI;

    });
}