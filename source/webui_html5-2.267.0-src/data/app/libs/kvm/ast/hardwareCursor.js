var hardwareCursor = function(){
	this.m_pattern = new Int16Array(64 * 64); //short
	this.cursorColorData = new Uint8Array(3); //byte
	this.canvas = document.getElementById('kvm');
	this.context = this.canvas.getContext('2d');
	this.previous_xpos=0;
	this.previous_ypos=0;
	this.CursorData =0;
	this.firstTime = true;
};

function updateHardwareCursor(m_ctrlMsg){
	readData(m_ctrlMsg);
	drawCursor();
}

/*
read cursor configurations from the packet 
m_ctrlMsg - Hardware cursor packet
*/
function readData(m_ctrlMsg){
	/* read first 13 byte */
	buffer = m_ctrlMsg.readUint8Array(13);
	this.cursorType = buffer[0];//byte
	this.checksum = (buffer[1] | buffer[2]<<8 | buffer[3]<<16 | buffer[4]<<24);//int
	this.xpos = (buffer[5] | buffer[6]<<8 );//short
	this.ypos = (buffer[7] | buffer[8]<<8 );//short
	this.xoffset = (buffer[9] | buffer[10]<<8 );//short
	this.yoffset = (buffer[11] | buffer[12]<<8 );//short

	buffer = null;
	delete buffer;

	/* read cursor pattern from the m_ctrlMsg only if packet contain cursor pattern value */
	if(m_ctrlMsg.byteLength > 13 && m_ctrlMsg.isEof() != true){
		/* Reading m_pattern(short) value from byte array */
		this.m_pattern = m_ctrlMsg.readInt16Array( parseInt( (m_ctrlMsg.byteLength-13)/2) ); //short 
	}
}

function drawPreviousCursorArea() {
	if(this.firstTime != true && this.previousCursorArea != null){
		this.context.putImageData(this.previousCursorArea,this.previous_xpos,this.previous_ypos,0,0,this.previous_cursorWidth,this.previous_cursorHeight);
		this.previousCursorArea = null;
		delete this.previousCursorArea;
	}
	else{
		this.firstTime = false;
	}
}

/*
draw host cursor
*/
function drawCursor() {

	drawPreviousCursorArea();
	this.cursorWidth = ((this.canvas.width- this.xpos) < 32) ? (this.canvas.width- this.xpos) : (64 - this.xoffset);
	this.cursorHeight = ((this.canvas.height- this.ypos) < 32) ? (this.canvas.height- this.ypos) : 32;

	this.previous_xpos=this.xpos;
	this.previous_ypos=this.ypos;
	this.previous_cursorWidth=this.cursorWidth;
	this.previous_cursorHeight= this.cursorHeight
	this.currentCursorArea = this.context.getImageData(this.xpos,this.ypos,this.cursorWidth,this.cursorHeight);
	var offset = 0;

	for ( x = 0; x < (64 - this.yoffset); x++) {

		if ((x + this.ypos) >= this.canvas.height) {
			continue;
		}

		for ( y = 0; y <(64 - this.xoffset) ; y++) {

			if ((y + this.xpos) >= this.canvas.width) {
				continue;
			}

			this.CursorData = this.m_pattern[(x + this.yoffset) * 64 + (y + this.xoffset)];
			this.cursorColorData[0] =  ((this.CursorData & 0x0F00) >> 4);
			this.cursorColorData[1] =  (this.CursorData & 0x00F0);
			this.cursorColorData[2] =  ((this.CursorData & 0x000F) << 4);

			if (this.cursorType == 1) {
				Alpha = (this.CursorData & 0xF000) >> 12;
				this.currentCursorArea.data[offset + 2] = ( ( ( (15 - Alpha) * this.currentCursorArea.data[offset + 2] ) / 15 ) + ((Alpha * this.cursorColorData[0]) / 15));
				this.currentCursorArea.data[offset + 1] = ( ( ( (15 - Alpha) * this.currentCursorArea.data[offset + 1] )/ 15 )+ ((Alpha * this.cursorColorData[1]) / 15));	
				this.currentCursorArea.data[offset + 0] = ( ( ( (15 - Alpha) * this.currentCursorArea.data[offset]) / 15) + ((Alpha * this.cursorColorData[2] )/ 15));
			}
			else {
				//	Get ANDBit, XORBit
				ANDBit = ((this.CursorData & 0x8000) >> 15);
				XORBit = ((this.CursorData & 0x4000) >> 14);
				if (ANDBit == 0) { // ANDBit == 0  ---> Cursor R/G/B
					this.currentCursorArea.data[offset + 2] = this.cursorColorData[0];
					this.currentCursorArea.data[offset + 1] = this.cursorColorData[1];
					this.currentCursorArea.data[offset + 0] = this.cursorColorData[2];
				} else {
					if (XORBit != 0) { // ANDBit == 1 && XORBit == 1 ---> NOT Graphics R/G/B
						this.currentCursorArea.data[offset + 2] = ~this.currentCursorArea.data[offset + 2];
						this.currentCursorArea.data[offset + 1] = ~this.currentCursorArea.data[offset + 1];
						this.currentCursorArea.data[offset] =  ~this.currentCursorArea.data[offset];
					}
				}
			}
			 offset = offset +4;
		}
	}
	saveCurrentCursorArea();
	updateImageBuffer();
}

/*
draw the current host cursor data
*/
function updateImageBuffer(){
	/*update only the cursor area on canvas*/
	this.context.putImageData(this.currentCursorArea,this.xpos,this.ypos,0,0,this.cursorWidth,this.cursorHeight);
	this.currentCursorArea = null;
	delete this.currentCursorArea;
}

/*
save current cursorArea to update previous cursor
*/
function saveCurrentCursorArea() {
	if( this.canvasData == null )
		return;
	this.curWidth = (this.cursorWidth <0 )? (-(this.cursorWidth)) : (this.cursorWidth);
	this.curHeight = (this.cursorHeight <0 )? (-(this.cursorHeight)) : (this.cursorHeight);
	if( this.curWidth >32 || this.curHeight >32){
		this.curWidth=0;
		this.curHeight=0;
	}
	this.previousCursorArea = this.context.getImageData(this.xpos,this.ypos,this.cursorWidth, this.cursorHeight);
	offset=0;
	x= this.xpos;
	/* Store canvas data to previousCursorArea */
	for ( y=this.ypos ; y< (this.ypos+this.curHeight); y++ ){
		var index = 4*(x+ ( y * this.canvas.width ));
		/* Read the data row by row*/
		var temp = this.canvasData.data.subarray(index, index+( (this.curWidth )*4 ) );
		this.previousCursorArea.data.set(temp, offset);
		offset = offset+temp.byteLength;
	}
}

/*
Update canvas data if video packet is received 
*/
function updateCanvasData() {
	if( _video.imageBuffer != null ){
		this.canvasData = _video.imageBuffer;
	}
	if( this.firstTime == false){
		saveCurrentCursorArea();
	}
}
