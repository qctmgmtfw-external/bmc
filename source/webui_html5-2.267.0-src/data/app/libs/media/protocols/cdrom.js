var CDROMProtocol = function() {

};

CDROMProtocol.prototype = {
    getPacket: function(buffer) {
        var header = new IUSBHeader();
        var packet = null;

        header.read(buffer);
        packet = new IUSBSCSI(header);
        packet.readData(buffer);

        return packet;
    }
};

if (typeof define == "function" && define.amd) {
    define(function() {
        return CDROMProtocol;

    });
}