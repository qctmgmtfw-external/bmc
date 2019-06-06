var IUSBSCSIPacket = function(request) {
	
	request.seek(0);

	this.header = new IUSBHeader(request);
	//console.log("scsi pkt header", this.header);
	this.readLength = request.readUint32();
	//console.log("scsi pkt readLength", this.readLength);
	this.tagNo = request.readUint32();
	//console.log("scsi pkt tagno", this.tagNo);
	this.dataDir = request.readUint8();
	//console.log("scsi pkt dataDir", this.dataDir);
	this.commandPacket = new SCSICommandPacket(request); //12 len (opcode, LUN, LBA4, resd, len2, resd3 )
	this.statusPacket = new SCSIStatusPacket(request); //4 len
	this.dataLength = request.readUint32();
	//console.log("scsi pkt dataLength", this.dataLength);
	this.data = request.readUint8();
	//console.log("scsi pkt data", this.data);

	request.seek(0);
};

if (typeof define == "function" && define.amd) {
    define(function() {
        return IUSBSCSIPacket;

    });
}