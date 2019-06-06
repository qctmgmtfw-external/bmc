var SCSICommandPacket = function(request) {

    if (request instanceof DataStream) {

        this.opcode = request.readUint8() & 0xff;
        //console.log("scsi pkt opcode", this.opcode);
        this.LUN = request.readUint8();
        //console.log("scsi pkt LUN", this.LUN);
        this.LBA = request.readUint32(DataStream.BIG_ENDIAN);
        //console.log("scsi pkt LBA", this.LBA);

        if (this.opcode == SCSICommandPacket.SCSI_READ_10 || this.opcode == SCSICommandPacket.SCSI_READ_TOC) {
            this.cmd10 = new CMD10();
            this.cmd10.reserved6 = request.readUint8();
            //console.log("scsi pkt cmd10 reserved6", this.cmd10.reserved6);
            this.cmd10.length = request.readUint16(DataStream.BIG_ENDIAN);
            //console.log("scsi pkt cmd10 length", this.cmd10.length);
            this.cmd10.reserved9.writeUint8Array(request.readUint8Array(3));
            //console.log("scsi pkt cmd10 reserved9");
        } else if (this.opcode == SCSICommandPacket.SCSI_READ_12) {
            this.cmd12 = new CMD12();
            this.cmd12.length32 = request.readUint32();
            //console.log("scsi pkt cmd12 length32", this.cmd12.length32);
            this.cmd12.reserved10.writeUint8Array(request.readUint8Array(2));
            //console.log("scsi pkt cmd12 reserved10");
        }

    } else if (request instanceof SCSICommandPacket) {
        this.opcode = request.opcode;
        //console.log("scsi pkt opcode", this.opcode);
        this.LUN = request.LUN;
        //console.log("scsi pkt LUN", this.LUN);
        this.LBA = request.LBA;
        //console.log("scsi pkt LBA", this.LBA);
        this.cmd10.reserved6 = request.cmd10.reserved6;
        //console.log("scsi pkt cmd10 reserved6", this.cmd10.reserved6);
        this.cmd10.length = request.cmd10.length;
        //console.log("scsi pkt cmd10 length", this.cmd10.length);
        this.cmd10.reserved9 = request.cmd10.reserved9;
        //console.log("scsi pkt cmd10 reserved9");
        this.cmd12.length32 = request.cmd12.length32;
        //console.log("scsi pkt cmd12 length32", this.cmd12.length32);
        this.cmd12.reserved10 = request.cmd12.reserved10;
        //console.log("scsi pkt cmd12 reserved10");
    }

};

SCSICommandPacket.SCSI_TEST_UNIT_READY = 0x00;
SCSICommandPacket.SCSI_READ_CAPACITY = 0x25;
SCSICommandPacket.SCSI_READ_10 = 0x28;
SCSICommandPacket.SCSI_READ_12 = 0xA8;
SCSICommandPacket.SCSI_READ_TOC = 0x43;
SCSICommandPacket.SCSI_START_STOP_UNIT = 0x1B;
SCSICommandPacket.SCSI_MEDIUM_REMOVAL = 0x1E;


var SCSIStatusPacket = function(request) {

    if (request instanceof DataStream) {

        this.overallStatus = request.readUint8();
        this.senseKey = request.readUint8();
        this.senseCode = request.readUint8();
        this.senseCodeQ = request.readUint8();

    } else if (request instanceof SCSIStatusPacket) {
        this.overallStatus = request.overallStatus;
        this.senseKey = request.senseKey;
        this.senseCode = request.senseCode;
        this.senseCodeQ = request.senseCodeQ;
    }

};

var CMD10 = function() {
    this.reserved9 = new DataStream(3);
    this.reserved6 = 0;
    this.length = 0;
};

var CMD12 = function() {
    this.reserved10 = new DataStream(2);
    this.length32 = 0;
};

if (typeof define == "function" && define.amd) {
    define(function() {
        return SCSICommandPacket;

    });
}