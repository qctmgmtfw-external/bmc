var CDImage = function(file) {

    this.dataLength = 0;
    this.append = new DataStream(20, 0, DataStream.LITTLE_ENDIAN);
    this.isFirstTime = true;
    this.opened = false;

    if (!file) {
        throw new Error("No valid file chosen");
    }

    this.imageFile = file;

    var _cdimage = this;

    this.validateISOImage(function(status) {
        if (status == CDImage.SUCCESS) {
            //console.log("ISO file is valid");
            _cdimage.setOpened.call(_cdimage, true);
        } else {
            //console.log("Not a valid ISO file");
            _cdimage.setOpened.call(_cdimage, false);
        }
    });

    this.totalSectors = 0;
    this.blockSize = 0;

    this.mediaChange = true;

};

CDImage.CD_BLOCK_SIZE = 2048;
CDImage.ISO_PRIMARY_VOLUME_DESCRIPTOR_LBA = 16;
CDImage.UDF_DOMAIN_ID_LBA = 35;
CDImage.UDF_DOMAIN_ID_OFFSET = 217;
CDImage.ISO_ID_SIZE = 5;
CDImage.UDF_DOMAIN_ID_SIZE = 19;
CDImage.ISO_ID = "CD001";
CDImage.UDF_DOMAIN_ID = "*OSTA UDF Compliant";
CDImage.MSF_BIT = 0x02;
CDImage.DATA_LENGTH_INDEX = 57;
CDImage.CURRENT_INDEX = 61;
CDImage.OVERALL_STATUS_INDEX = 53;
CDImage.SENSE_KEY_INDEX = 54;
CDImage.SENSE_CODE_INDEX = 55;
CDImage.SENSE_CODE_Q_INDEX = 56;

CDImage.SUCCESS = 0;
CDImage.SECTOR_RANGE_ERROR = -1;
CDImage.WRONG_MEDIA = -2;
CDImage.MEDIUM_CHANGE = -3;
CDImage.MEDIA_ERROR = -4;
CDImage.NO_MEDIA = -5;
CDImage.INVALID_PARAMS = -6;
CDImage.UNREADABLE_MEDIA = -7;
CDImage.REMOVAL_PREVENTED = -8;
CDImage.UNSUPPORTED_COMMAND = -9;
CDImage.DEVICE_PATH_TOO_LONG = -10;
CDImage.DEVICE_ERROR = -11;
CDImage.DEVICE_ALREADY_OPEN = -12;
CDImage.MEDIUM_GETTING_READY = -14;
CDImage.MEDIA_IN_USE = -15;


CDImage.prototype = {

    isOpened: function() {
        return this.opened;
    },

    setOpened: function(status) {
        this.opened = status;
    },

    validateISOImage: function(callback) {
        var ret = CDImage.SUCCESS;
        var isoIDBuffer = new DataStream(CDImage.ISO_ID_SIZE);
        var udfDomainIDData = new DataStream(CDImage.UDF_DOMAIN_ID_SIZE);
        var isoID = null;
        var udfDomainID = null;
        this.blockSize = CDImage.CD_BLOCK_SIZE;

        var _cdimage = this;

        var isoreader = new FileReader();

        isoreader.onloadend = function(e) {
            if (e.target.readyState == FileReader.DONE) {
                ////console.log(new Uint8Array(e.target.result));
                //document.getElementById("log").innerHTML += ("<br/>read bytes at "+ (new Date()));
                var blockBuffer = new DataStream(e.target.result);
                if (!blockBuffer.byteLength) {
                    throw new Error("ISO file invalid");
                }

                ////console.log("file block", blockBuffer.readUint8Array());

                blockBuffer.seek(1);
                if (blockBuffer.readString(CDImage.ISO_ID_SIZE) == CDImage.ISO_ID) {
                    callback(CDImage.SUCCESS);
                } else {
                    //check if UDF image
                    var udf_details_pos = CDImage.UDF_DOMAIN_ID_LBA * CDImage.CD_BLOCK_SIZE;
                    var udfblob = _cdimage.imageFile.slice(iso_details_pos, iso_details_pos + CDImage.CD_BLOCK_SIZE);

                    var udfreader = new FileReader();

                    udfreader.onloadend = function(e) {

                        var blockBuffer = new DataStream(e.target.result);
                        if (!blockBuffer.byteLength) {
                            throw new Error("ISO file invalid");
                        }

                        blockBuffer.seek(CDImage.UDF_DOMAIN_ID_OFFSET);
                        if (blockBuffer.readString(CDImage.UDF_DOMAIN_ID_SIZE) == CDImage.UDF_DOMAIN_ID) {
                            callback(CDImage.SUCCESS);
                        } else {
                            callback(CDImage.WRONG_MEDIA);
                        }


                    };

                    udfreader.readAsArrayBuffer(udfblob);

                }
            }
        };

        //ISO Image primary volume descriptor is in LBA 16
        var iso_details_pos = CDImage.ISO_PRIMARY_VOLUME_DESCRIPTOR_LBA * CDImage.CD_BLOCK_SIZE;
        var isoblob = this.imageFile.slice(iso_details_pos, iso_details_pos + CDImage.CD_BLOCK_SIZE);
        isoreader.readAsArrayBuffer(isoblob);
    },

    readCapacity: function() {
        var ret = CDImage.SUCCESS;
        this.blockSize = CDImage.CD_BLOCK_SIZE;
        this.totalSectors = Math.floor(this.imageFile.size / CDImage.CD_BLOCK_SIZE);

        return ret;
    },

    readCDImage: function(sectorNo, numSectors, callback) {

        //console.log("sectorNo", sectorNo, "numSectors", numSectors);

        var data = new DataStream(CDImage.CD_BLOCK_SIZE * numSectors);
        var fromSector = CDImage.CD_BLOCK_SIZE * sectorNo;
        var toSector = fromSector + (CDImage.CD_BLOCK_SIZE * numSectors);

        var reader = new FileReader();
        reader.onloadend = function(e) {
            if (e.target.readyState == FileReader.DONE) {
                callback(e.target.result); //return arraybuffer
            }
        };

        var blob = this.imageFile.slice(fromSector, toSector);
        reader.readAsArrayBuffer(blob);

    },

    executeSCSICmd: function(requests, response, callback) {
        var val = CDImage.SUCCESS;
        var packet = null;
        var readBuffer = null;
        var startingSectorNumber = 0;
        var numberOfSectors = 0;
        var maxTOCSize = 0;
        var requestPacket = null;


        var reqPos = requests.position;
        requests.seek(0);
        var temp = requests.readUint8Array();
        requests.seek(reqPos);
        response.writeUint8Array(temp);

        response.seek(0);
        ////console.log("response ", response.readUint8Array(), response.byteLength);

        requestPacket = new IUSBSCSIPacket(requests);

        var _cdimage = this;


        packet = requestPacket.commandPacket;

        //console.log("packet.opcode", packet.opcode);

	switch (packet.opcode) {
	    case SCSICommandPacket.SCSI_TEST_UNIT_READY:
		if(this.isFirstTime == true)
		{
			this.isFirstTime = false;
			val = CDImage.MEDIUM_CHANGE;
		}
		else{
			val = this.testUnitReady();
		}
		response.seek(CDImage.DATA_LENGTH_INDEX);
		response.writeUint8(0);
		this.dataLength = 0;
		break;

            case SCSICommandPacket.SCSI_READ_CAPACITY:
                //console.log("SCSI CMD readCapacity");
                val = this.readCapacity();
                response.seek(CDImage.DATA_LENGTH_INDEX);
                response.writeUint8(8);

                response.seek(CDImage.CURRENT_INDEX);
                response.writeUint32(this.totalSectors - 1, DataStream.BIG_ENDIAN);
                response.writeUint32(this.blockSize, DataStream.BIG_ENDIAN);

                this.dataLength = 8;
                break;

            case SCSICommandPacket.SCSI_READ_10:
            case SCSICommandPacket.SCSI_READ_12:
                //console.log("SCSI CMD readCDImage");
                startingSectorNumber = packet.LBA;
                numberOfSectors = ((packet.opcode == SCSICommandPacket.SCSI_READ_10) ? packet.cmd10.length : packet.cmd12.length32);

                /*
                if (startingSectorNumber >= 0) {
                    startingSectorNumber = this.mac2blong(packet.LBA);
                } else {
                    // mac2blong doesn't work as expected for negative numbers.
                    // // for that we are shifting the bits as needed.
                    var ts = new DataStream(8);

                    ts.writeUint32(startingSectorNumber);
                    ts.seek(0);
                    var tb = ts.readUint8Array();
                    tb[0] = tb[1] = tb[2] = tb[3] = 0;

                    var tb6 = tb[6],
                        tb7 = tb[7];
                    tb[7] = tb[4];
                    tb[6] = tb[5];
                    tb[4] = tb7;
                    tb[5] = tb6;

                    ts = new DataStream(tb.buffer);
                    startingSectorNumber = ts.readUint32();
                }
                numberOfSectors = this.mac2bshort(packet.cmd10.length);
                */


                this.readCDImage(startingSectorNumber, numberOfSectors, function(readBuffer) {
                    response.seek(CDImage.DATA_LENGTH_INDEX);
                    response.writeUint32(readBuffer.byteLength);
                    //TODO decide to write readbuffer
                    _cdimage.dataLength = readBuffer.byteLength;

                    //console.log("writing byteLength", readBuffer.byteLength);
                    response.seek(CDImage.CURRENT_INDEX);
                    response.writeUint8Array(new Uint8Array(readBuffer));

                    callback(CDImage.SUCCESS);
                });

                return;

            case SCSICommandPacket.SCSI_READ_TOC:
                //console.log("SCSI CMD readTOC");
                response.seek(CDImage.DATA_LENGTH_INDEX);
                response.writeUint8(20); //TOC data length is 20

                this.readTOC(packet, function(status, toc) {
                    maxTOCSize = packet.cmd10.length;
                    if (_cdimage.dataLength > maxTOCSize) {
                        _cdimage.dataLength = maxTOCSize;
                    }

                    response.seek(CDImage.DATA_LENGTH_INDEX);
                    response.writeUint8(_cdimage.dataLength);


                    _cdimage.setErrorStatus(response, status);

                    response.seek(CDImage.CURRENT_INDEX);
                    response.writeUint8Array(toc);

                    callback(status);
                });

                return;

            case SCSICommandPacket.SCSI_START_STOP_UNIT:
                //console.log("SCSI CMD SCSI_START_STOP_UNIT");
                val = CDImage.SUCCESS;
                break;

            case SCSICommandPacket.SCSI_MEDIUM_REMOVAL:
                //console.log("SCSI CMD SCSI_MEDIUM_REMOVAL");
                val = CDImage.UNSUPPORTED_COMMAND;
                break;
        }

        this.setErrorStatus(response, val);

        return callback(val);

    },

    testUnitReady: function() {
        return CDImage.SUCCESS;
    },

    readTOC: function(packet, callback) {
        var startTrack,
            nLength = 4,
            TOC = new Uint8Array(20),
            allocatedLength = 0;

        startTrack = packet.cmd10.reserved6;
        allocatedLength = packet.cmd10.length;
        

        if (startTrack > 1 && startTrack != 0xaa) {
            callback(0);
        }

        TOC[2] = 1;
        TOC[3] = 1;

        if (startTrack <= 1) {
            TOC[nLength++] = 0; //reserved;
            TOC[nLength++] = 0x14; //ADR, control
            TOC[nLength++] = 1; //Track number
            TOC[nLength++] = 0; //reserved

            if ((packet.LUN & CDImage.MSF_BIT) == 1) {
                //Sense in MSF Format
                // Logical ABS ADDR = (((MIN*60)+SEC)*75 + FRAC) - 150
                TOC[nLength++] = 0; //reserved
                TOC[nLength++] = 0; //minute
                TOC[nLength++] = 2; //second
                TOC[nLength++] = 0; //frame
            } else {
                //Sense Direct Absolute Address
                TOC[nLength++] = 0; //reserved
                TOC[nLength++] = 0; //minute
                TOC[nLength++] = 0; //second
                TOC[nLength++] = 0; //frame

            }

        }

        TOC[nLength++] = 0; //reserved
        TOC[nLength++] = 0x16; //ADR. control
        TOC[nLength++] = 0xaa; //Track number
        TOC[nLength++] = 0; //reserved

        TOC[nLength++] = 0;
        TOC[nLength++] = (((this.totalSectors + 150) / 75) / 60); //minute
        TOC[nLength++] = (((this.totalSectors + 150) / 75) % 60); //second
        TOC[nLength++] = (((this.totalSectors + 150) % 75)); //frame

        if (nLength > allocatedLength) {
            nLength = allocatedLength;
        }

        TOC[0] = (((nLength - 2) >> 8) & 0xff);
        TOC[1] = ((nLength - 2) & 0xff);
        this.dataLength = nLength;
        return callback(CDImage.SUCCESS, TOC);
    },

    setErrorStatus: function(response, val) {
        switch (val) {
            case CDImage.SUCCESS:
                response.seek(CDImage.OVERALL_STATUS_INDEX);
                response.writeUint8(0);
                response.seek(CDImage.SENSE_KEY_INDEX);
                response.writeUint8(0);
                response.seek(CDImage.SENSE_CODE_INDEX);
                response.writeUint8(0);
                response.seek(CDImage.SENSE_CODE_Q_INDEX);
                response.writeUint8(0);
                break;
            case CDImage.SECTOR_RANGE_ERROR:
                response.seek(CDImage.OVERALL_STATUS_INDEX);
                response.writeUint8(1);
                response.seek(CDImage.SENSE_KEY_INDEX);
                response.writeUint8(0x05);
                response.seek(CDImage.SENSE_CODE_INDEX);
                response.writeUint8(0x21);
                response.seek(CDImage.SENSE_CODE_Q_INDEX);
                response.writeUint8(0);
                break;
            case CDImage.WRONG_MEDIA:
                response.seek(CDImage.OVERALL_STATUS_INDEX);
                response.writeUint8(0);
                response.seek(CDImage.SENSE_KEY_INDEX);
                response.writeUint8(0x03);
                response.seek(CDImage.SENSE_CODE_INDEX);
                response.writeUint8(0x30);
                response.seek(CDImage.SENSE_CODE_Q_INDEX);
                response.writeUint8(0x01);
                break;
            case CDImage.MEDIUM_CHANGE:
                response.seek(CDImage.OVERALL_STATUS_INDEX);
                response.writeUint8(1);
                response.seek(CDImage.SENSE_KEY_INDEX);
                response.writeUint8(0x06);
                response.seek(CDImage.SENSE_CODE_INDEX);
                response.writeUint8(0x28);
                response.seek(CDImage.SENSE_CODE_Q_INDEX);
                response.writeUint8(0);
                break;
            case CDImage.MEDIA_ERROR:
                response.seek(CDImage.OVERALL_STATUS_INDEX);
                response.writeUint8(1);
                response.seek(CDImage.SENSE_KEY_INDEX);
                response.writeUint8(0x03);
                response.seek(CDImage.SENSE_CODE_INDEX);
                response.writeUint8(0x11);
                response.seek(CDImage.SENSE_CODE_Q_INDEX);
                response.writeUint8(0);
                break;
            case CDImage.NO_MEDIA:
                response.seek(CDImage.OVERALL_STATUS_INDEX);
                response.writeUint8(1);
                response.seek(CDImage.SENSE_KEY_INDEX);
                response.writeUint8(0x02);
                response.seek(CDImage.SENSE_CODE_INDEX);
                response.writeUint8(0x3A);
                response.seek(CDImage.SENSE_CODE_Q_INDEX);
                response.writeUint8(0);
                break;
            case CDImage.INVALID_PARAMS:
                response.seek(CDImage.OVERALL_STATUS_INDEX);
                response.writeUint8(1);
                response.seek(CDImage.SENSE_KEY_INDEX);
                response.writeUint8(0x05);
                response.seek(CDImage.SENSE_CODE_INDEX);
                response.writeUint8(0x26);
                response.seek(CDImage.SENSE_CODE_Q_INDEX);
                response.writeUint8(0);
                break;
            case CDImage.UNREADABLE_MEDIA:
                response.seek(CDImage.OVERALL_STATUS_INDEX);
                response.writeUint8(1);
                response.seek(CDImage.SENSE_KEY_INDEX);
                response.writeUint8(0x03);
                response.seek(CDImage.SENSE_CODE_INDEX);
                response.writeUint8(0x30);
                response.seek(CDImage.SENSE_CODE_Q_INDEX);
                response.writeUint8(0x02);
                break;
            case CDImage.REMOVAL_PREVENTED:
                response.seek(CDImage.OVERALL_STATUS_INDEX);
                response.writeUint8(1);
                response.seek(CDImage.SENSE_KEY_INDEX);
                response.writeUint8(0x05);
                response.seek(CDImage.SENSE_CODE_INDEX);
                response.writeUint8(0x53);
                response.seek(CDImage.SENSE_CODE_Q_INDEX);
                response.writeUint8(0x02);
                break;
            case CDImage.UNSUPPORTED_COMMAND:
            default:
                response.seek(CDImage.OVERALL_STATUS_INDEX);
                response.writeUint8(1);
                response.seek(CDImage.SENSE_KEY_INDEX);
                response.writeUint8(0x05);
                response.seek(CDImage.SENSE_CODE_INDEX);
                response.writeUint8(0x20);
                response.seek(CDImage.SENSE_CODE_Q_INDEX);
                response.writeUint8(0);
                break;
        }
    },

    mac2bshort: function(x) {
        return ((x >> 8) | (x << 8));
    },

    mac2blong: function(x) {
        return ((x >> 24) | (x << 24) | ((x & 0x00ff0000) >> 8) | ((x & 0x0000ff00) << 8));
    },

    closeImage: function() {
        this.imageFile = null;
    }

};

if (typeof define == "function" && define.amd) {
    define(function() {
        return CDImage;

    });
}