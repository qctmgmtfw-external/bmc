var touchEventListener = function(_video) {

    $("#kbd").show();
    $("#kvm_textbox").hide().blur();
    document.getElementById('kbd').onclick = function(e) {
        if ($(this).data('show')) {
            $(document).unbind('click');
            $("#kvm_textbox").hide().blur();
            $(this).data('show', 0).html('Show KBD');
        } else {
            _video.$canvas.click(function() {
                $("#kvm_textbox").show().focus();
            });
            $("#kvm_textbox").show().focus();
            $(this).data('show', 1).html('Hide KBD');
        }
    };

    var usbkbdmsg = new USBKeyBoardMessage(),
        usbengkbd = new USBKeyBoardEnglish();

    usbkbdmsg.setUSBKeyProcessor(usbengkbd);

    var usbmouse = new USBMouseMessage();

    _video.textbox.onkeyup = function(e) {
        if (_video.running) {
            var keyCode = e.keyCode,
                keyLocation = (e.keyLocation !== undefined ? e.keyLocation : e.location),
                keyPressed = false;

            if (keyCode == 229 || keyCode === 0) {
                handleSoftKeyboard(e);
                return;
            } else {
                e.stopPropagation();
                e.preventDefault();
            }


            usbkbdmsg.set(keyCode, keyLocation, keyPressed);
            var rpt = usbkbdmsg.report();

            _video.sock.send(rpt.buffer);
            //delete rpt;
            //TODO:need to set modifiers correctly
            if (!e.altKey && !e.shiftKey && !e.ctrlKey) {
                usbengkbd.setModifiers(0 | window.keyboard_mask);
            }
        }
    };

    _video.textbox.onkeypress = function(e) {
        //alert("pressed");
        //e.stopPropagation();
        //e.preventDefault();
    };

    _video.textbox.onkeydown = function(e) {
        if (_video.running) {
            var keyCode = e.keyCode,
                keyLocation = (e.keyLocation !== undefined ? e.keyLocation : e.location),
                keyPressed = true;

            if (keyCode == 229 || keyCode === 0) {
                return;
            } else {
                e.stopPropagation();
                e.preventDefault();
            }

            usbkbdmsg.set(keyCode, keyLocation, keyPressed);
            var rpt = usbkbdmsg.report();
            _video.sock.send(rpt.buffer);
            //delete rpt;
        }
    };





    MOUSE = {
        LBUTTON_DOWN: 0x01,
        MBUTTON_DOWN: 0x04,
        RBUTTON_DOWN: 0x02
    };

    var btnStatus = 0;

    this.onhold = function(e) {

        var x = e.touches[0].pageX - _video.$canvas.offset().left,
            y = e.touches[0].pageY - _video.$canvas.offset().top;

        //console.log("hold");

        clearTimeout(releaseTimer);
        releaseTimer = setTimeout(function() {
            touchContext.onrelease.call(touchContext, e);
        }, 80);

        switch (1) {
            case 1:
                btnStatus |= MOUSE.LBUTTON_DOWN;
                break;
            case 2:
                btnStatus |= MOUSE.MBUTTON_DOWN;
                break;
            case 3:
                btnStatus |= MOUSE.RBUTTON_DOWN;
                break;
        }
        usbmouse.set(btnStatus, x, y, window.mousemode, _video.$canvas.width(), _video.$canvas.height(), 0);
        var rpt = usbmouse.report();
        _video.sock.send(rpt.buffer);
        //delete rpt;
    };

    this.onrelease = function(e) {
        var x = e.touches[0].pageX - _video.$canvas.offset().left,
            y = e.touches[0].pageY - _video.$canvas.offset().top;

        $(_video.textbox).css('left', (x + 10) + 'px').css('top', (y + 10) + 'px');
        //console.log("release");
        switch (1) {
            case 1:
                btnStatus &= ~MOUSE.LBUTTON_DOWN;
                break;
            case 2:
                btnStatus &= ~MOUSE.MBUTTON_DOWN;
                break;
            case 3:
                btnStatus &= ~MOUSE.RBUTTON_DOWN;
                break;
        }
        usbmouse.set(btnStatus, x, y, window.mousemode, _video.$canvas.width(), _video.$canvas.height(), 0);
        var rpt = usbmouse.report();
        _video.sock.send(rpt.buffer);
        //delete rpt;
    };

    var holdTimer = null;
    var touchContext = this;

    _video.canvas.ontouchstart = function(e) {
        //console.log(e);

        if (e.touches.length > 1) return;

        e.stopPropagation();
        e.preventDefault();
        //(btnStatus, x, y, mode, width, height, wheelrotation)
        if (e.touches.length == 1) {
            clearTimeout(holdTimer);
            holdTimer = setTimeout(function() {
                touchContext.onhold.call(touchContext, e);
            }, 50);

            var x = e.touches[0].pageX - _video.$canvas.offset().left,
                y = e.touches[0].pageY - _video.$canvas.offset().top;

            $(_video.textbox).css('left', (x + 10) + 'px').css('top', (y + 10) + 'px');
            switch (1) {
                case 1:
                    btnStatus &= ~MOUSE.LBUTTON_DOWN;
                    break;
                case 2:
                    btnStatus &= ~MOUSE.MBUTTON_DOWN;
                    break;
                case 3:
                    btnStatus &= ~MOUSE.RBUTTON_DOWN;
                    break;
            }
            //console.log('start ' + btnStatus);

            usbmouse.set(btnStatus, x, y, window.mousemode, _video.$canvas.width(), _video.$canvas.height(), 0);
            var rpt = usbmouse.report();
            _video.sock.send(rpt.buffer);
            //delete rpt;
        }
    };


    _video.canvas.ontouchcancel = function(e) {
        if (e.touches.length > 2) return;

        if (e.touches.length == 1) {
            //console.log(e);
            e.stopPropagation();
            e.preventDefault();

            var x = e.touches[0].pageX - _video.$canvas.offset().left,
                y = e.touches[0].pageY - _video.$canvas.offset().top;

            clearTimeout(holdTimer);
            clearTimeout(releaseTimer);
            //$(_video.textbox).css('left', e.pageX+'px').css('top', e.pageY+'px');

            switch (1) {
                case 1:
                    btnStatus &= ~MOUSE.LBUTTON_DOWN;
                    break;
                case 2:
                    btnStatus &= ~MOUSE.MBUTTON_DOWN;
                    break;
                case 3:
                    btnStatus &= ~MOUSE.RBUTTON_DOWN;
                    break;
            }

            //console.log('cancel ' + btnStatus);
            usbmouse.set(btnStatus, x, y, window.mousemode, _video.$canvas.width(), _video.$canvas.height(), 0);
            var rpt = usbmouse.report();
            _video.sock.send(rpt.buffer);
            //delete rpt;
        }
    };

    _video.canvas.ontouchend = function(e) {
        if (e.touches.length > 2) return;

        if (e.touches.length == 1) {
            //console.log(e);
            e.stopPropagation();
            e.preventDefault();

            var x = e.touches[0].pageX - _video.$canvas.offset().left,
                y = e.touches[0].pageY - _video.$canvas.offset().top;

            clearTimeout(holdTimer);
            clearTimeout(releaseTimer);
            //$(_video.textbox).css('left', e.pageX+'px').css('top', e.pageY+'px');

            switch (1) {
                case 1:
                    btnStatus &= ~MOUSE.LBUTTON_DOWN;
                    break;
                case 2:
                    btnStatus &= ~MOUSE.MBUTTON_DOWN;
                    break;
                case 3:
                    btnStatus &= ~MOUSE.RBUTTON_DOWN;
                    break;
            }
            //console.log('end ' + btnStatus);
            usbmouse.set(btnStatus, x, y, window.mousemode, _video.$canvas.width(), _video.$canvas.height(), 0);
            var rpt = usbmouse.report();
            _video.sock.send(rpt.buffer);
            //delete rpt;
        }
    };

    var releaseTimer = null;

    var scroll = {
        dir: 1,
        y: -1
    };

    _video.canvas.ontouchmove = function(e) {
        if (e.touches.length > 2) return;

        e.stopPropagation();
        e.preventDefault();

        clearTimeout(holdTimer);
        clearTimeout(releaseTimer);

        var rpt = null,
            x, y;

        if (e.touches.length == 1) {
            //console.log(e)

            x = e.touches[0].pageX - _video.$canvas.offset().left;
            y = e.touches[0].pageY - _video.$canvas.offset().top;


            releaseTimer = setTimeout(function() {
                touchContext.onrelease.call(touchContext, e);
            }, 100);
            //console.log('move ' + btnStatus);

            //usbmouse.set(btnStatus, x, y, window.mousemode, _video.$canvas.width(), _video.$canvas.height(),  0);
            usbmouse.set(1, x, y, window.mousemode, _video.$canvas.width(), _video.$canvas.height(), 0);
            rpt = usbmouse.report();
            _video.sock.send(rpt.buffer);
            //delete rpt;
        } else {
            //2 touches
            x = e.touches[0].pageX - _video.$canvas.offset().left;
            y = e.touches[0].pageY - _video.$canvas.offset().top;

            if (scroll.y != -1) {
                if (scroll.y < y) {
                    scroll.dir = 1;
                } else {
                    scroll.dir = -1;
                }
            }

            scroll.y = y;

            usbmouse.set(btnStatus, x, y, window.mousemode, _video.$canvas.width(), _video.$canvas.height(), scroll.dir);
            rpt = usbmouse.report();
            _video.sock.send(rpt.buffer);
            //delete rpt;
        }
    };


    var handleSoftKeyboard = function() {
        var currentText = $(_video.textbox).val();
        var rpt = null;
        //alert(currentText);
        for (var i = 0; i < currentText.length; i++) {
            var keyCode = softkeys['k_' + currentText.charAt(i)][0],
                keyLocation = 0;
            //keyPressed = false;

            if (keyCode === undefined) continue;

            //alert(currentText.charAt(i))

            if (softkeys['k_' + currentText.charAt(i)][2]) {
                //shift
                usbkbdmsg.set(16, 1, true);
                rpt = usbkbdmsg.report();
                _video.sock.send(rpt.buffer);
                //delete rpt;
            }
            /*
			if(currentText.charAt(i).toLowerCase()!=currentText.charAt(i)) {
				//CAPS
				usbkbdmsg.set(20, 0, true);				
				var rpt = usbkbdmsg.report();
				if(_video.sock.wsmode=="binary") {
					_video.sock.send(rpt.buffer);
				} else {
					_video.sock.send(base64ArrayBuffer(rpt.buffer));
				}
				delete rpt;
			}*/

            usbkbdmsg.set(keyCode, keyLocation, true);
            rpt = usbkbdmsg.report();
            _video.sock.send(rpt.buffer);
            //delete rpt;

            if (softkeys['k_' + currentText.charAt(i)][2]) {
                //shift
                usbkbdmsg.set(16, 1, false);
                rpt = usbkbdmsg.report();
                _video.sock.send(rpt.buffer);
                //delete rpt;
            }
            /*
			if(currentText.charAt(i).toLowerCase()!=currentText.charAt(i)) {
				//CAPS
				usbkbdmsg.set(20, 0, false);				
				var rpt = usbkbdmsg.report();
				if(_video.sock.wsmode=="binary") {
					_video.sock.send(rpt.buffer);
				} else {
					_video.sock.send(base64ArrayBuffer(rpt.buffer));
				}
				delete rpt;
			}*/

            usbkbdmsg.set(keyCode, keyLocation, false);
            rpt = usbkbdmsg.report();
            _video.sock.send(rpt.buffer);
            //delete rpt;
        }
        $(_video.textbox).val("").blur().focus();
    };
};
if (typeof define == "function" && define.amd) {
    define(function() {
        return touchEventListener;

    });
}