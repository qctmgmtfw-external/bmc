var CDROM = function() {
    this.sock = null;
    this.protocol = new CDROMProtocol();
    this.inBufferSize = CDROM.MAX_READ_DATA_SIZE;
    this.outBufferSize = CDROM.MAX_READ_SIZE + IUSBHeader.HEADER_LEN + IUSBSCSI.IUSB_SCSI_PKT_SIZE_WITHOUT_HEADER;
    this.cdImage = null;
    this.stopRunning = false;
};

CDROM.DEVICE_REDIRECTION_ACK = 0xf1;
CDROM.AUTH_CMD = 0xf2;
CDROM.DEVICE_INFO = 0xf8;
CDROM.CONNECTION_ACCEPTED = 1;
CDROM.CONNECTION_LOGIN_FAILED = 3;
CDROM.CONNECTION_IN_USE = 4;
CDROM.CONNECTION_PERM_DENIED = 5;
CDROM.CONNECTION_MAX_USER = 8;
CDROM.MAX_READ_SECTORS = 0x40;
CDROM.MAX_READ_SIZE = 2048 * CDROM.MAX_READ_SECTORS;
CDROM.MAX_READ_DATA_SIZE = 1024;

CDROM.prototype = {
    connect: function(host, port, useSSL) {
        this.sock = new MediaSocket(host, port, this.protocol, useSSL);
        this.sock.setBuffer(0, this.outBufferSize, DataStream.LITTLE_ENDIAN, DataStream.LITTLE_ENDIAN);

        this.sock.connect();

        var _cdrom = this;

        this.sock.onopen = function(e) {
            _cdrom.continueRedirection.call(_cdrom);
        };
    },

    startRedirection: function(host, cdromDrive, token, port, useSSL) {

        this._token = token;

        this.connect(host, port, useSSL);

    },

    continueRedirection: function() {

        var _cdrom = this;

        //attach auth message handler
        this.sock.onmessage = function(e) {
            var request = _cdrom.sock.receivePacket();

            //console.log("special packet ", request);

            _cdrom.cdInstance = request.instance;

            if (request.opcode == CDROM.DEVICE_REDIRECTION_ACK) {
                switch (request.connectionStatus) {
                    case CDROM.CONNECTION_PERM_DENIED:
                        notify(window.CommonStrings.cdError_noPrivilege, false, NOTIFICATION.DANGER);
                        $("#media").click();
                        return;

                    case CDROM.CONNECTION_LOGIN_FAILED:
                        notify(window.CommonStrings.cdError_loginFailed, false, NOTIFICATION.DANGER);
                        $("#media").click();
                        return;

                    case CDROM.CONNECTION_MAX_USER:
                        notify(window.CommonStrings.cdError_maxUser, false, NOTIFICATION.DANGER);
                        $("#media").click();
                        return;

                    case CDROM.CONNECTION_IN_USE:
                        if( request.otherIP != null){
                            //For device redirected using LMedia/RMedia 
                            if( ( request.otherIP.localeCompare("127.0.0.1") == 0 ) || (request.otherIP.localeCompare("::1") == 0 ) ) {
                                notify(window.CommonStrings.cdError_connectionInUse_LMedia_RMedia, false, NOTIFICATION.DANGER);
                            }
                            else{
                                //For device redirection by Other user
                                notify(window.CommonStrings.cdError_connectionInUse_Other_User+" "+request.otherIP,false, NOTIFICATION.DANGER);
                            }
                            $("#media").click();
                        }
                        return;

                    case CDROM.CONNECTION_ACCEPTED:
                        //console.log("connected");
                        if (request.otherIP.length) {
                            //console.log("Another IP connected " + request.otherIP);
                        }
                        break;

                }
            }


            _cdrom.emulate.call(_cdrom);


        };

        this.authenticate(this._token);
        this.sendMediaInfo();
    },

    authenticate: function(token) {
        //console.log("Authenticating...");
        var len = IUSBSCSI.IUSB_SCSI_PKT_SIZE + IUSBHeader.HEADER_LEN + IUSBSession.WEB_AUTH_PKT_MAX_SIZE;

        var ab = this.sock.outBuffer.buffer.slice(0, len + 1);
        this.sock.outBuffer = new DataStream(ab);

        //console.log("dataLength", len);

        var auth_pkt = new IUSBHeader(len);
        auth_pkt.write(this.sock.outBuffer);

        this.sock.outBuffer.seek(41); //OpCode SCSI CMD PKT
        this.sock.outBuffer.writeUint8(CDROM.AUTH_CMD & 0xff);

        this.sock.outBuffer.seek(IUSBSCSI.IUSB_SCSI_PKT_SIZE);
        this.sock.outBuffer.writeUint8(0); //authpacket flag
        this.sock.outBuffer.writeString(token);

        this.sock.outBuffer.seek(23);
        this.sock.outBuffer.writeUint8(this.CDDeviceNo);

        this.sock.outBuffer.seek(0);

        var scsi_pkt = new IUSBSCSI(this.sock.outBuffer, true);
        this.sock.sendPacket(scsi_pkt);

    },

    sendMediaInfo: function(){
        var len = IUSBSCSI.IUSB_SCSI_PKT_SIZE - IUSBHeader.HEADER_LEN + IUSBSession.DEVICE_INFO_MAX_SIZE;

        var ab = this.sock.outBuffer.buffer.slice(0, len + 1);
        this.sock.outBuffer = new DataStream(ab);

        var media_info = new IUSBHeader(len);
        media_info.write(this.sock.outBuffer);

        this.sock.outBuffer.seek(IUSBSCSI.IUSB_SCSI_OPCODE_INDEX); //OpCode SCSI CMD PKT
        this.sock.outBuffer.writeUint8(CDROM.DEVICE_INFO & 0xff);
        this.sock.outBuffer.seek(IUSBSCSI.IUSB_SCSI_PKT_SIZE);
        this.sock.outBuffer.writeUint32(window.H5VIEWER);// 3 for H5Viewer
        this.sock.outBuffer.writeString(window.file.name+'\0');
        this.sock.outBuffer.seek(0);
        var scsi_pkt = new IUSBSCSI(this.sock.outBuffer, true);
        this.sock.sendPacket(scsi_pkt);

    },

    emulate: function() {
        var _cdrom = this;

        var request, response, dataLength = 0,
            tempSentLength = 0;

        var onSCSICMDExecuted = function(status) {
            dataLength = _cdrom.cdImage.dataLength + IUSBHeader.IUSB_HEADER_SIZE;

            //console.log("total data length", dataLength, "cd", _cdrom.cdImage.dataLength);

            if (dataLength >= 0 && dataLength <= _cdrom.sock.outBuffer.byteLength) {
                var ab = _cdrom.sock.outBuffer.buffer.slice(0, dataLength);
                _cdrom.sock.outBuffer = new DataStream(ab);
                //console.log("resetting outBuffer length", _cdrom.sock.outBuffer.byteLength);
            }

            //response byte buffer modified in executeSCSICmd function, so reset to 0
            _cdrom.sock.outBuffer.seek(0);

            if (request.opcode == IUSBSCSI.OPCODE_KILL_REDIR) {
                console.log("Session Terminated ");
                $("#media").click();
                return;
            }

            else if (request.opcode == IUSBSCSI.MEDIA_SESSION_DISCONNECT) {
                console.log("Media Service Restarted");
                $("#media").click();
                return;
            }

	    //Send a Keep Alive command in response to the one received from the cdserver.
            else if (request.opcode == IUSBSCSI.KEEP_ALIVE) {
            	_cdrom.sendKeepAlivePkt();
            	return;
            }


            response = new IUSBSCSI(_cdrom.sock.outBuffer, true);
            _cdrom.sock.sendPacket(response); //send IUSBSCSI packet

            tempSentLength += dataLength;

            _cdrom.nBytesRedirected += (tempSentLength / 1024);

            tempSentLength = tempSentLength % 1024;

            //handle eject command after sending response to host to avoid host throw error on eject
            if (request.opcode == IUSBSCSI.OPCODE_EJECT) {
                if (request.LBA == IUSBSCSI.LBA_EJECTED) {
                    console.log("CD Image Ejected");
                    $("#media").click();
                    return;
                }
            }
        };

        //attach 
        this.sock.onmessage = function(e) {

            delete _cdrom.sock.outBuffer;
            _cdrom.sock.outBuffer = new DataStream(this.outBufferSize);

            //while (!this.stopRunning) {
            request = _cdrom.sock.receivePacket();

            if (request === null) {
                return;
            }

            //executeSCSI command
            _cdrom.cdImage.executeSCSICmd(_cdrom.sock.inBuffer, _cdrom.sock.outBuffer, onSCSICMDExecuted);


            //}


        };

        //console.log("image file ", file.size);

        //Open the image file
        this.cdImage = new CDImage(file);

        this.nBytesRedirected = 0;
        this.sock.outBuffer.seek(0);
    },

    onclose: function() {
        if (window.onmediaclose) {
            window.onmediaclose();
        }
    },

    sendstopcommandtoserver: function() {
        //console.log("sendstopcommandtoserver...");
        window.cdrom.sock.outBuffer.seek(41); //OpCode SCSI CMD PKT
        window.cdrom.sock.outBuffer.writeUint8(IUSBSCSI.MEDIA_SESSION_DISCONNECT & 0xff);
        window.cdrom.sock.outBuffer.seek(0);

        var  response = new IUSBSCSI( window.cdrom.sock.outBuffer, true);
        window.cdrom.sock.sendPacket(response); //send IUSBSCSI packet

    },
    
    /**
    * Send Keep Alive IUSB command to the cdserver.
    */
    sendKeepAlivePkt: function() {
        window.cdrom.sock.outBuffer.seek(41); //OpCode SCSI CMD PKT
        window.cdrom.sock.outBuffer.writeUint8(IUSBSCSI.KEEP_ALIVE & 0xff);
        window.cdrom.sock.outBuffer.seek(0);

        var  response = new IUSBSCSI( window.cdrom.sock.outBuffer, true);
        window.cdrom.sock.sendPacket(response); //send IUSBSCSI packet

    }
};

if (typeof define == "function" && define.amd) {
    define(function() {
        return CDROM;

    });
}