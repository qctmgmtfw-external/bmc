define(['underscore', 'backbone',
        'i18n!strings/nls/units',
        'i18n!strings/nls/discrete_states',
        'i18n!strings/nls/sensor_specific_states',
        'i18n!strings/nls/threshold_states'
    ],
    function(_, Backbone, units, discrete_states, sensor_specific_states, threshold_states) {

        var model = Backbone.Model.extend({

            unit: function() {
                //return units[this.get('base_unit')];
                var u = this.get('unit');
                switch(u) {
                    case 'deg_c':
                        //u = '&deg;C';//Quanta
                    	u = '°C';//Quanta
                    break;
                    default:
                        u = u.split('');
                        u[0]  = u[0].toUpperCase();
                        u = u.join('');
                }
            	return u;
            },

            name: function() {
                return this.get('name').replace(/[^\x20-\x7E]+/g, '');;
            },

            state: function() {
                var st = "",
                    bitpos = 0;

                if (this.get('type_number') >= 0xC0 && this.get('type_number') <= 0xFF) {
					if(this.get('reading')==0x8000)
						return "All Deasserted";
					else
					    return "Asserted("+this.get('reading').toString(16)+"h)";
                }

                if (this.get('sensor_state')) {

                    for (bitpos = 0; bitpos < 8; bitpos++) {
                        if (this.get('sensor_state') & (0x01 << bitpos)) {
                            st += threshold_states["threshold_" + (0x01 << bitpos)] + " ";
                        }
                    }
                    console.log(st);
                } else {
                    if (this.get('sensor_number') == 12)
                        console.log('PCM sensor_specific_' + this.get('type_number') + '_' + bitpos);
                    for (bitpos = 0; bitpos < 8 && this.get('reading'); bitpos++) {
                        if (this.get('reading') & (0x01 << bitpos)) {
                            //console.log('reading bit set');
                            if (this.get('discrete_state') >= 0x02 && this.get('discrete_state') <= 0x0c) {
                                st += discrete_states['generic_discrete_' + this.get('discrete_state') + '_' + bitpos] + " ";
                            } else if (0x6F == this.get('discrete_state')) {
                                st += sensor_specific_states['sensor_specific_' + this.get('type_number') + '_' + bitpos] + " ";
                            } else if (this.get('discrete_state') >= 0x70 && this.get('discrete_state') <= 0x7F) {
                                //TODO: check OEM specific information..
                                st = "Manufacturer Specific";
                            }
                            //console.log(st);
                        }
                    }
                    if (!st) {
                        st = "All Deasserted";
                    }
                }
                console.log(st);
                return st;
            },

            sensorType: function() {

            	if(typeof this.get('type') == 'string') return this.get('type');

                switch (this.get('type')) {
                    case 1:
                        return "Temperature";
                    case 2:
                        return "Voltage";
                    case 3:
                        return "Current";
                    case 4:
                        return "Fan";
                    case 5:
                        return "Chassis Intrusion";
                    case 6:
                        return "Platform Security Violation";
                    case 7:
                        return "Processor";
                    case 8:
                        return "Power Supply";
                    case 9:
                        return "Power Unit";
                    case 10:
                        return "Cooling Device";
                    case 11:
                        return "Other Units";
                    case 12:
                        return "Memory";
                    case 13:
                        return "Drive Slot";
                    case 14:
                        return "Post Memory Resize";
                    case 15:
                        return "System Firmware Error";
                    case 16:
                        return "Event Logging Disabled";
                    case 17:
                    case 35:
                        return "Watchdog";
                    case 18:
                        return "System Event";
                    case 19:
                        return "Critical Interrupt";
                    case 20:
                        return "Button";
                    case 21:
                        return "Board";
                    case 22:
                        return "Microcontroller";
                    case 23:
                        return "Add In Card";
                    case 24:
                        return "Chassis";
                    case 25:
                        return "Chipset";
                    case 26:
                        return "Other FRU";
                    case 27:
                        return "Cable";
                    case 28:
                        return "Terminator";
                    case 29:
                        return "System Boot";
                    case 30:
                        return "Boot Error";
                    case 31:
                        return "Base OS Boot";
                    case 32:
                        return "OS Shutdown";
                    case 33:
                        return "Slot";
                    case 34:
                        return "System ACPI";
                    case 36:
                        return "Platform Alert";
                    case 37:
                        return "Entity Presence";
                    case 38:
                        return "Monitor ASIC";
                    case 39:
                        return "LAN";
                    case 40:
                        return "Management Subsystem Health";
                    case 41:
                        return "Battery";
                    case 42:
                        return "Session Audit";
                    case 43:
                        return "Version Change";
                    case 44:
                        return "FRU State";
                }
                if (this.get('type') >= 0xC0 && this.get('type') <= 0xFF) {
                    return "oem_reserved";
                }
            },

            hnr: function() {
                return this.get('higher_non_recoverable_threshold');
            },

            hc: function() {
                return this.get('higher_critical_threshold');
            },

            hnc: function() {
                return this.get('higher_non_critical_threshold');
            },

            lnc: function() {
                return this.get('lower_non_critical_threshold');
            },

            lc: function() {
                return this.get('lower_critical_threshold');
            },

            lnr: function() {
                return this.get('lower_non_recoverable_threshold');
            },

            reading: function() {
                return this.get('reading');
            },
            sdrType: function(){
                return this.get('sdr_type');
            },//Quanta+

            getPercent: function() {
                var l = -this.lnr();
                var h = this.hnr() + l;
                var r = this.reading() + l;

                return Math.ceil((r / h) * 100);
            },

            isCritical: function() {
                if ((this.get('reading') >= this.get('higher_non_critical_threshold')) ||
                    (this.get('reading') <= this.get('lower_non_critical_threshold'))) {
                    return true;
                } else {
                    return false;
                }
            },

            isNormal: function() {
                if ((this.get('reading') < this.get('higher_non_critical_threshold')) &&
                    (this.get('reading') > this.get('lower_non_critical_threshold'))) {
                    return true;
                } else {
                    return false;
                }
            }

        });

        return model;

    });
