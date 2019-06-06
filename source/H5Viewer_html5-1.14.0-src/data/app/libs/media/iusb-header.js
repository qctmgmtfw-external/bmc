var IUSBHeader = function(dataLength) {

    if (typeof dataLength == "number") {
        this.setSignature();
        this.key = new Uint8Array(4);
        this.headerLength = IUSBHeader.HEADER_LEN;
        this.major = IUSBHeader.IUSB_MAJOR;
        this.minor = IUSBHeader.IUSB_MINOR;
        this.sequenceNo = 0;
        this.direction = 0x80;
        this.dataPacketLength = dataLength || 0;
        this.deviceType = 0x05;
        this.deviceNo = 0;
        this.interfaceNo = 0;
        this.instance = 0;
        this.protocol = 0x01;
    } else if (dataLength instanceof DataStream) {
        var buffer = dataLength;
        buffer.seek(0);
        //console.log("header buffer", buffer.readUint8Array());
        buffer.seek(0);

        this.signature = buffer.readString(IUSBHeader.IUSB_SIGNATURE_SIZE);
        this.major = buffer.readUint8() & 0xff;
        this.minor = buffer.readUint8() & 0xff;
        this.headerLength = buffer.readUint8() & 0xff;
        this.headerCheckSum = buffer.readUint8() & 0xff;
        this.dataPacketLength = buffer.readUint32() & 0xffffffff;
        this.serverCaps = buffer.readUint8() & 0xff;
        this.deviceType = buffer.readUint8() & 0xff;
        this.protocol = buffer.readUint8() & 0xff;
        this.direction = buffer.readUint8() & 0xff;
        this.deviceNo = buffer.readUint8() & 0xff;
        this.interfaceNo = buffer.readUint8() & 0xff;
        this.clientData = buffer.readUint8() & 0xff;
        this.instance = buffer.readUint8() & 0xff;
        this.sequenceNo = buffer.readUint32() & 0xffffffff;
        this.key = buffer.readUint8Array(4);
    }

    this.headerLength = IUSBHeader.HEADER_LEN;

};

IUSBHeader.IUSB_SIGNATURE_SIZE = 8;
IUSBHeader.IUSB_HEADER_SIZE = 61;
IUSBHeader.HEADER_LEN = 32;
IUSBHeader.IUSB_HEADER = "IUSB    ";
IUSBHeader.IUSB_MAJOR = 1;
IUSBHeader.IUSB_MINOR = 0;

IUSBHeader.prototype = {
    setSignature: function(signature) {
        this.signature = signature || IUSBHeader.IUSB_HEADER;
    },

    write: function(buffer) {

        buffer.seek(0);
        buffer.writeString(this.signature);
        buffer.writeUint8(this.major & 0xff);
        buffer.writeUint8(this.minor & 0xff);
        buffer.writeUint8(this.headerLength & 0xff);
        buffer.writeUint8(this.headerCheckSum & 0xff);
        buffer.writeUint32(this.dataPacketLength & 0xffffffff);
        buffer.writeUint8(this.serverCaps & 0xff);
        buffer.writeUint8(this.deviceType & 0xff);
        buffer.writeUint8(this.protocol & 0xff);
        buffer.writeUint8(this.direction & 0xff);
        buffer.writeUint8(this.deviceNo & 0xff);
        buffer.writeUint8(this.interfaceNo & 0xff);
        buffer.writeUint8(this.clientData & 0xff);
        buffer.writeUint8(this.instance & 0xff);
        buffer.writeUint32(this.sequenceNo & 0xffffffff);
        buffer.writeUint8Array(this.key);

        /* calculate checksum */
        buffer.seek(0);
        var temp = 0;
        for (var i = 0; i < buffer.byteLength; i++) {
            temp = (temp + (buffer.readUint8() & 0xff)) & 0xff;
        }

        buffer.seek(11);
        buffer.writeInt8(-(temp & 0xff));

        return buffer;
    },

    read: function(buffer) {


        this.signature = buffer.readString(IUSBHeader.IUSB_SIGNATURE_SIZE);
        if (this.signature != IUSBHeader.IUSB_HEADER) {
            throw new Error("IUSB Header Mismatch while reading", this.signature);
        }
        this.major = buffer.readUint8() & 0xff;
        this.minor = buffer.readUint8() & 0xff;
        this.headerLength = buffer.readUint8() & 0xff;
        this.headerCheckSum = buffer.readUint8() & 0xff;
        this.dataPacketLength = buffer.readUint32() & 0xffffffff;
        this.serverCaps = buffer.readUint8() & 0xff;
        this.deviceType = buffer.readUint8() & 0xff;
        this.protocol = buffer.readUint8() & 0xff;
        this.direction = buffer.readUint8() & 0xff;
        this.deviceNo = buffer.readUint8() & 0xff;
        this.interfaceNo = buffer.readUint8() & 0xff;
        this.clientData = buffer.readUint8() & 0xff;
        this.instance = buffer.readUint8() & 0xff;
        this.sequenceNo = buffer.readUint32() & 0xffffffff;
        this.key = buffer.readUint8Array(4);
    }
};

IUSBHeader.parse = function(buffer) {
    buffer.seek(0);
    var signature = buffer.readString(8);
    if (signature != IUSBHeader.IUSB_HEADER) {
        throw new Error("Not an IUSB Header");
    }

    buffer.seek(0);

    var header = buffer.readStruct([
        'signature', 'string:8',
        'major', 'uint8',
        'minor', 'uint8',
        'headerLength', 'uint8',
        'dataPacketLength', 'uint32',
        'serverCaps', 'uint8',
        'deviceType', 'uint8',
        'protocol', 'uint8',
        'direction', 'uint8',
        'deviceNo', 'uint8',
        'interfaceNo', 'uint8',
        'clientData', 'uint8',
        'instance', 'uint8',
        'sequenceNo', 'uint32',
        'key', ['[]', 'uint8', 4]
    ]);

    return header;
};

if (typeof define == "function" && define.amd) {
    define(function() {
        return IUSBHeader;

    });
}