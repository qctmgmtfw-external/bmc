define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//system_inventory_device_info',
'collection//system_inventory_cpu_info',
'collection//system_inventory_dimm_info',
'collection//system_inventory_pci_info',
//views
'views//system_inventory_info_item',
        //localize
        'i18n!strings/nls/system_inventory_info',
        //template
        'text!templates//system_inventory_info.html',
        //plugin
        'tooltip',
        'footable'
    ],

    function($, _, Backbone, app, SystemInventoryDeviceInfoCollection, SystemInventoryCPUInfoCollection, SystemInventoryDIMMInfoCollection, SystemInventoryPCIInfoCollection, SystemInventoryInfoItemView, locale, SystemInventoryInfoTemplate) {

        var view = Backbone.View.extend({

            template: _.template(SystemInventoryInfoTemplate),
		DIMM_CPU_NAME:[],
		DIMM_CHANNAL_NAME:[],
		DIMM_NAME: [],
		PCI_CPU_NAME: [],
		PCI_CHANNAL_NAME: [],
		PCI_NAME: [],
            CHANNAL_WISE_DIMM: [],
            CHANNAL_WISE_PCI: [],
            SYS_INVEN_COUNT: [],
            SYS_INVEN_CPU_INFO: [],
            SYS_INVEN_DIMM_INFO: [],
            SYS_INVEN_PCIe_INFO: [],
            CPU_INFO_NAME: [ "Brand Name:","Core Count:","CPU Family:","CPU Index:","CPU Model:","CPU Vendor:","Device Path:","Device Present:" ,"Frequency:","Stepping:","Thread Count:"],
            DIMM_INFO_NAME: ["Channel Number:","Device Path:","Device Present:","Frequency:","Slot:", "Size:", "Manufacturer ID:","Node Number:","Part Number:","Serial Number:"],

            initialize: function() {
                this.system_inventory_device_info = SystemInventoryDeviceInfoCollection;
                this.collection = this.system_inventory_device_info;
                this.system_inventory_device_info.bind('add', this.add, this);
                this.system_inventory_device_info.bind('reset', this.reset, this);
                this.system_inventory_device_info.bind('remove', this.removeModel, this);
                this.system_inventory_device_info.bind('change', this.change, this);
                this.system_inventory_device_info.bind('change add reset remove', this.affect, this);
            },

            events: {
            	"click .help-link": "toggleHelp"
            },

            beforeRender: function() {

            },

        afterRender: function() {


			var context = this;

			context.DIMM_CPU_NAME=new Array();
			context.DIMM_CHANNAL_NAME=new Array();
			context.DIMM_NAME=new Array();
			context.PCI_CPU_NAME=new Array();
			context.PCI_CHANNAL_NAME=new Array();
			context.PCI_NAME=new Array();
			context.CHANNAL_WISE_DIMM =new Array();
			context.CHANNAL_WISE_PCI=new Array();
			context.SYS_INVEN_COUNT =new Array();
                context.SYS_INVEN_CPU_INFO = new Array();
                context.SYS_INVEN_DIMM_INFO = new Array();
                context.SYS_INVEN_PCIe_INFO = new Array();

                this.collection.fetch({
                    success: function() {
                        context.load.call(context);
                    },
                    error: function() {
                        app.events.trigger('save_error', context);
                    }
                });
            },
            load: function(model, collection, xhr) {
                var that = this;
                that.SYS_INVEN_COUNT = $.parseJSON(JSON.stringify(that.collection));

                SystemInventoryCPUInfoCollection.fetch({
                    success: function() {
                        that.SYS_INVEN_CPU_INFO = $.parseJSON(JSON.stringify(SystemInventoryCPUInfoCollection));
                        //console.log(that.SYS_INVEN_CPU_INFO);
                        that.loadDIMM.call(that);
                    }
                });
            },
            loadDIMM: function() {
                var that = this;
                SystemInventoryDIMMInfoCollection.fetch({
                    success: function() {
                        that.SYS_INVEN_DIMM_INFO = $.parseJSON(JSON.stringify(SystemInventoryDIMMInfoCollection));
                        //console.log(" Dimm info "+that.SYS_INVEN_DIMM_INFO);
                        that.loadPCI.call(that);
                    }
                });
            },
            loadPCI: function() {
                var that = this;
                SystemInventoryPCIInfoCollection.fetch({
                    success: function() {
                        that.SYS_INVEN_PCIe_INFO = $.parseJSON(JSON.stringify(SystemInventoryPCIInfoCollection));
                        //console.log(that.SYS_INVEN_PCIe_INFO);
                        that.buildSystemInventory.call(that);
                    }
                });
            },
            buildSystemInventory: function() {
                var that = this;
                //console.log("length" +that.SYS_INVEN_COUNT.length);

                //console.log(that.SYS_INVEN_COUNT[0].CPUPresenceCount);
                var presenceCPUCount = that.SYS_INVEN_COUNT[0].CPUPresenceCount;

                for (var k = 0; k < presenceCPUCount; k++) {

                    var cpuname = "CPU_" + k;

                    that.buildCPUDIMM.call(that, cpuname);

                    that.buildDIMMChannals.call(that, cpuname);

					that.buildCPU.call(that,cpuname);

					that.parsePCIeInfo.call(that,cpuname);

					that.buildPCIChannals.call(that,cpuname);

					that.buildPCI.call(that,cpuname);

					that.DIMM_CPU_NAME = [];
					that.DIMM_CHANNAL_NAME = [];
					that.DIMM_NAME = [];
                    that.PCI_CPU_NAME = [];
                    that.PCI_CHANNAL_NAME = [];
                    that.PCI_NAME = [];
                    that.CHANNAL_WISE_DIMM = [];
                    that.CHANNAL_WISE_PCI = [];
                }
                that.displayPopup.call(that);
            },
            displayPopup: function() {
                var that = this;
                var presenceCPUCount = that.SYS_INVEN_COUNT[0].CPUPresenceCount;
                if (presenceCPUCount > 0) {
                    for (var k = 0; k < presenceCPUCount; k++) {
                        //console.log($('#divcpu_'+k));
                        if( ($('#divcpu_' + k) != null)&&(that.SYS_INVEN_CPU_INFO[k].DEVPRESENCE==1) ){
                            var titlestr = that.getCPUInfo.call(that, k);
                            $('#divcpu_' + k).tooltip({ title: titlestr, html: true, placement: "middle" });
                        }
                    }
                }

                if (presenceCPUCount > 0) {
                    for (var k = 0; k < presenceCPUCount; k++) {
                        for (var j = 0; j < that.SYS_INVEN_DIMM_INFO.length; j++) {
                            if ($('#iddimdiv' + k + "_dtab" + j) != null) {
                                if (that.SYS_INVEN_DIMM_INFO[j].DEVPRESENCE == 1) {
                                    var titlestr = that.getDIMMInfo.call(that, j);
                                    if(j>4)
                                    $('#iddimdiv' + k + " tr[index=" + that.SYS_INVEN_DIMM_INFO[j].INDEX + "]").tooltip({ title: titlestr, html: true, placement: "top" });
                                    else
                                    $('#iddimdiv' + k + " tr[index=" + that.SYS_INVEN_DIMM_INFO[j].INDEX + "]").tooltip({ title: titlestr, html: true, placement: "middle" });
                                    //$('#iddimdiv' + k + " tr[index=" + that.SYS_INVEN_DIMM_INFO[j].INDEX + "]").html(titlestr);
                                }
                            }
                        }
                    }
                }
            },
            getCPUInfo: function(index) {
                var that = this;
                var innerTable = "<table width='100%' border='1' cellpadding='0' cellspacing='0'><tr><td colspan='3'><b>CPU Information</b></td></tr>";

                for (var j = 0; j < that.SYS_INVEN_CPU_INFO.length; j++) {
                    if (index == that.SYS_INVEN_CPU_INFO[j].INDEX) {

                        var data = that.SYS_INVEN_CPU_INFO[j];

                        //"Brand Name :","Core Count :","CPU Family :","CPU Index :","CPU Model :","CPU Vendor :","Device Path :","Device Present :" ,"Max Frequency :","Stepping :","Thread Count :"

                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.CPU_INFO_NAME[0] + "</td>";
                        innerTable += "<td>" + data.BRANDNAME + "</td></tr>";

                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.CPU_INFO_NAME[1] + "</td>";
                        innerTable += "<td>" + data.CORECOUNT + "</td></tr>";
/*
                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.CPU_INFO_NAME[2] + "</td>";
                        innerTable += "<td>" + data.CPUFAMILY + "</td></tr>";
*/
                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.CPU_INFO_NAME[3] + "</td>";
                        innerTable += "<td>" + data.CPUINDEX + "</td></tr>";
/*
                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.CPU_INFO_NAME[4] + "</td>";
                        innerTable += "<td>" + data.CPUMODEL + "</td></tr>";

                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.CPU_INFO_NAME[5] + "</td>";
                        innerTable += "<td >" + data.CPUVENDOR + "</td></tr>";
*/
                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.CPU_INFO_NAME[6] + "</td>";
                        innerTable += "<td>" + data.DEVPATH + "</td></tr>";

                        var present=(data.DEVPRESENCE==1)?"Yes":"No";

                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.CPU_INFO_NAME[7] + "</td>";
                        innerTable += "<td>" + present + "</td></tr>";

                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.CPU_INFO_NAME[8] + "</td>";
                        innerTable += "<td>" + data.MAXFREQUENCY+" MHz" + "</td></tr>";
/*
                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.CPU_INFO_NAME[9] + "</td>";
                        innerTable += "<td>" + data.STEPPING + "</td></tr>";
*/
                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.CPU_INFO_NAME[10] + "</td>";
                        innerTable += "<td>" + data.THREADCOUNT + "</td></tr>";

                    }
                }
                innerTable += "</table>";

                //var str="<h1><strong>HTML</strong> inside <code>the</code> <em> this CPU" + k +"</em></h1>";
                return innerTable;
            },
            getDIMMInfo: function(index) {
                var that = this;
                var innerTable = "<table width='100%' border='1' cellpadding='0' cellspacing='0'><tr><td colspan='3'><b>DIMM Information</b></td></tr>";
                /*innerTable += "<tr><td colspan='2'>" + "CPU Name" +"</td>";
                innerTable += "<td>" + "INTEL" + "</td></tr>";
                innerTable += "<tr><td colspan='2'>" + "CPU Version" + "</td>";
                innerTable += "<td>" + k + "</td></tr>";*/
                //console.log(index);

                for (var j = 0; j < that.SYS_INVEN_DIMM_INFO.length; j++) {
                    if (index == that.SYS_INVEN_DIMM_INFO[j].INDEX) {

                        var data = that.SYS_INVEN_DIMM_INFO[j];

                        //"Channel Number","Device Path","Device Present","Frequency","Slot", "Size", "Manufacturer Name","Node Number","Part Number","Serial Number"

                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.DIMM_INFO_NAME[0] + "</td>";
                        innerTable += "<td>" + data.CHANNELNUMBER + "</td></tr>";

                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.DIMM_INFO_NAME[1] + "</td>";
                        innerTable += "<td>" + data.DEVPATH + "</td></tr>";

                        var present=(data.DEVPRESENCE==1)?"Yes":"No";

                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.DIMM_INFO_NAME[2] + "</td>";
                        innerTable += "<td>" + present + "</td></tr>";

                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.DIMM_INFO_NAME[3] + "</td>";
                        innerTable += "<td>" + data.DIMMFREQ +" MHz"+ "</td></tr>";


                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.DIMM_INFO_NAME[4] + "</td>";
                        innerTable += "<td>" + data.DIMMSLOT_INDEX + "</td></tr>";

                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.DIMM_INFO_NAME[5] + "</td>";
                        innerTable += "<td>" + data.DIMMSIZE +" MB"+ "</td></tr>";

                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.DIMM_INFO_NAME[6] + "</td>";
                        innerTable += "<td>" + data.MANUFACTURER_ID + "</td></tr>";

                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.DIMM_INFO_NAME[7] + "</td>";
                        innerTable += "<td>" + data.NODENUMBER + "</td></tr>";

                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.DIMM_INFO_NAME[8] + "</td>";
                        innerTable += "<td>" + data.PARTNUMBER + "</td></tr>";

                        var serial_number= data.SERIALNUMBER_1 + "-" + data.SERIALNUMBER_2 +"-" + data.SERIALNUMBER_3 +"-" +data.SERIALNUMBER_4;
                        innerTable += "<tr><td colspan='2' style='white-space:nowrap'>" + that.DIMM_INFO_NAME[9] + "</td>";
                        innerTable += "<td>" + serial_number + "</td></tr>";

                    }
                }
                innerTable += "</table>";
                return innerTable;
            },
            buildCPUDIMM: function(cpuname) {
                var that = this;
                //console.log("buildCPUDIMM" +that.SYS_INVEN_DIMM_INFO.length);

                for (i = 0; i < that.SYS_INVEN_DIMM_INFO.length; i++) {
                    var data = that.SYS_INVEN_DIMM_INFO[i];

                    var dpath = data.DEVPATH;
                    var splitpath = dpath.split("/");
                    var dpresence = data.DEVPRESENCE;
                    var dindex = data.INDEX;
    				if (that.DIMM_CHANNAL_NAME.indexOf(splitpath[1].replace("(", "").replace(")", "")) == -1)
                    {
    					if(cpuname==splitpath[0].replace("(", "").replace(")", "").trim())
                        {
                            that.DIMM_CPU_NAME.push(splitpath[0].replace("(", "").replace(")", ""));
    						that.DIMM_CHANNAL_NAME.push(splitpath[1].replace("(", "").replace(")", ""));
                        }
                    }
                    that.DIMM_NAME.push(splitpath[2].replace("(", "").replace(")", "") + "-" + dpresence + "-" + dindex);
                }
                //console.log("that.DIMM_NAME.length = "+that.DIMM_NAME.length);

                var dimm_num_per_cpu = (that.DIMM_NAME.length / that.SYS_INVEN_COUNT[0].CPUPresenceCount);
                var dimm_index = 0;
                var dimm_range = 0;
                var cpu_num = cpuname.replace(/[^0-9]/ig,"");
                cpu_num = parseInt(cpu_num);

                if(!isNaN(cpu_num)){
                	dimm_index=(dimm_num_per_cpu*cpu_num);
                	dimm_range=(dimm_num_per_cpu*(cpu_num+1));
                }
                //console.log("Cpu name: " + cpuname + " num: " + cpu_num + " dimm :" + dimm_index + " rang: "+ dimm_range);

                if (that.DIMM_CHANNAL_NAME.length > 0) {
                    for (i = 0; i < that.DIMM_CHANNAL_NAME.length; i++) {
                        if (that.DIMM_NAME.length > 0) {
                            for (j = dimm_index; j < dimm_range; j++) {
                                var dcha = that.DIMM_NAME[j].split(" ")[1];
                                if (that.DIMM_CHANNAL_NAME[i].trim() == dcha.trim())
                                {
                                    that.CHANNAL_WISE_DIMM.push(that.DIMM_CHANNAL_NAME[i] + "-" +
                                    that.DIMM_NAME[j].split(" ")[2] + "-" + that.DIMM_CPU_NAME[i].replace("(", "").replace(")", "") + "-" + that.DIMM_NAME[j].split("-")[1] + "-" + that.DIMM_NAME[j].split("-")[2]);
                                }
                            }
                        }
                    }
                }

                if (that.CHANNAL_WISE_DIMM.length > 0) {

                    var divdimm = document.createElement("DIV");
                    divdimm.className = "dimmdiv";
                    divdimm.id = "iddimdiv" + cpuname.split("_")[1];


	            for (k = 0; k < that.DIMM_CHANNAL_NAME.length; k++) {
	                var cha = that.DIMM_CHANNAL_NAME[k];
	                var tab = document.createElement("TABLE");
	                tab.className = "dimmtab";
	                tab.id = "iddimdiv" + cpuname.split("_")[1] +"_"+ "dtab" + k;
	                tab.style.marginTop = "20px";
	                tab.border = 1;

	                for (l = 0; l < that.CHANNAL_WISE_DIMM.length; l++) {
	                    var cha1 = that.CHANNAL_WISE_DIMM[l].split("-")[0];
	                    if (cha == cha1) {
                                var tr = document.createElement("TR");
                                tr.setAttribute("index", that.CHANNAL_WISE_DIMM[l].split("-")[3]);


                                var td = document.createElement("TD");
                                var dp = that.CHANNAL_WISE_DIMM[l].split("-")[2];



                                if (that.CHANNAL_WISE_DIMM[l].split("-")[2] == 1) {
                                    tr.className = "dimmtr";
                                    tr.onmouseover = function() {
                                        this.style.backgroundColor = "Gold";
                                    }
                                    tr.onmouseout = function() {
                                        this.style.backgroundColor = "Green";
                                    }
                                    tr.onclick = function() {

                                    }

                                } else {

                                    tr.className = "";
                                }
                                var lbl = document.createElement("LABEL");
                                lbl.className = "dimmlbl";
                                lbl.innerHTML = that.CHANNAL_WISE_DIMM[l].split("-")[1];

	                        td.appendChild(lbl);
                                tr.appendChild(td);
                                tab.appendChild(tr);
                            }
                        }
                        divdimm.appendChild(tab);
                    }
                    $('#iddivSystemInventory').append(divdimm);
                }
            },
            buildDIMMChannals: function(cpuname) {
                var that = this;

                if (that.DIMM_CHANNAL_NAME.length > 0) {

                    var no = cpuname.split("_")[1];

                    for (i = 0; i < that.DIMM_CHANNAL_NAME.length; i++) {

					var divChaDIMM = document.createElement("DIV");
					divChaDIMM.className = "dimmchannal";
					divChaDIMM.id = "dimmchannal" + i;

					var dimtab= document.getElementById("iddimdiv" + no +"_"+ "dtab" +i);
						if(dimtab != null){
							//console.log("dimtab.offsetTop" + dimtab.offsetTop);
							//console.log("dimtab.offsetLeft" + dimtab.offsetLeft);
							var top= dimtab.offsetTop + 45 + "px";
							var left= dimtab.offsetLeft + 35 + "px";
							divChaDIMM.style.marginTop = top;
							divChaDIMM.style.marginLeft =left;
						}

                        var lbl = document.createElement("LABEL");
                        lbl.className = "dimmchannallbl";
                        lbl.innerHTML = that.DIMM_CHANNAL_NAME[i];

                        divChaDIMM.appendChild(lbl);

                        $('#iddivSystemInventory').append(divChaDIMM);
                    }
                }
            },
            buildCPU: function(cpuname) {

                var that = this;
                var i = cpuname.split("_")[1];

                var divMain = document.createElement("DIV");
                divMain.className = "cpudiv";
                divMain.id = "divcpu_" + i;

            var tab = document.createElement("TABLE");
            tab.className = "cputab";
            tab.id = "ctab" + i;
            tab.style.marginLeft = "80px";

            var totaldimmObj=document.getElementById("iddimdiv" + i);
            if(totaldimmObj != null){
            	//console.log("totaldimmObj" + totaldimmObj.offsetHeight);
                    tab.style.height = totaldimmObj.offsetHeight + "px";
                }

                var tr = document.createElement("TR");
                tr.setAttribute("index", i);
                tr.onclick = function() {

                }

                var td = document.createElement("TD");
                td.className = "cputd";

                var lbl = document.createElement("LABEL");
            lbl.className = "cpulbl";
            lbl.innerHTML = cpuname;

                td.appendChild(lbl);
                tr.appendChild(td);
                tab.appendChild(tr);

                divMain.appendChild(tab);

                $('#iddivSystemInventory').append(divMain);
            },
            parsePCIeInfo: function(cpuname) {

                var that = this;
                if (that.SYS_INVEN_PCIe_INFO.length > 0) {
                    for (i = 0; i < that.SYS_INVEN_PCIe_INFO.length; i++) {
                        var data = that.SYS_INVEN_PCIe_INFO[i];
                        var dpath = data.DEVPATH;
                        var splitpath = dpath.split("/");
						var dpresence=data.DEVPRESENCE;
						var dindex= data.INDEX;
						that.PCI_CPU_NAME.push(splitpath[0].replace("(", "").replace(")", ""));
						if(cpuname==splitpath[0].replace("(", "").replace(")", "").trim()){
							that.PCI_CHANNAL_NAME.push(splitpath[1].replace("(", "").replace(")", ""));
						}
						if(cpuname==splitpath[0].replace("(", "").replace(")", "").trim()){
							that.PCI_NAME.push(splitpath[2].replace("(", "").replace(")", "") + "-" + dpresence + "-" +dindex);
						}
					}
					if (that.PCI_CHANNAL_NAME.length > 0) {
						for (i = 0; i < that.PCI_CHANNAL_NAME.length; i++) {
                            that.CHANNAL_WISE_PCI.push(that.PCI_CHANNAL_NAME[i] + "-" + that.PCI_NAME[i] + "-" + that.PCI_CPU_NAME[i].replace("(", "").replace(")", "") + "-" + that.PCI_NAME[i].split("-")[1] + "-" + that.PCI_NAME[i].split("-")[2]);
                        }
                    }
                }
            },
            buildPCIChannals: function(cpuname) {

                var that = this;
                if (that.PCI_CHANNAL_NAME.length > 0) {

                    var no = cpuname.split("_")[1];
                    for (i = 0; i < that.PCI_CHANNAL_NAME.length; i++) {
                        var divChaDIMM = document.createElement("DIV");
						divChaDIMM.className = "pcichannal";
						divChaDIMM.id = cpuname + "pcichannal" + i;
						//divChaDIMM.style.marginTop = (i == 0) ? "50px" : (45 + (i * 100)) + "px";

						var dimtab= document.getElementById("iddimdiv" + no +"_"+ "dtab" +i);
							if(dimtab != null){
								//console.log("pci dimtab.offsetTop" + dimtab.offsetTop);
								//console.log("pci dimtab.offsetLeft" + dimtab.offsetLeft);

								var top= dimtab.offsetTop + 30 + "px";
								var left= dimtab.offsetLeft + 215 + "px";
								divChaDIMM.style.marginTop = top;
								divChaDIMM.style.marginLeft =left;
							}



						var lbl = document.createElement("LABEL");
						lbl.className = "pcichannallbl";
						lbl.innerHTML = that.PCI_CHANNAL_NAME[i];

						divChaDIMM.appendChild(lbl);
						//console.log(divChaDIMM);
						$('#iddivSystemInventory').append(divChaDIMM);
					}
				}
		},
		buildPCI:function(cpuname){
		//console.log("inside buildPCI1");
		var that=this;
		if (that.SYS_INVEN_PCIe_INFO.length > 0) {

				if (that.CHANNAL_WISE_PCI.length > 0) {

					var no= cpuname.split("_")[1];

					var divpci = document.createElement("DIV");
					divpci.className = "pcidiv";

					for (i = 0; i < that.CHANNAL_WISE_PCI.length; i++) {

						var tab = document.createElement("TABLE");
						tab.className = "pcitab";
                            tab.id = "ptab" + i;
                            tab.border = 1;
                            if (i == 0) {
                                tab.style.marginTop = 40 + "px";
                                tab.style.marginLeft = 90 + "px";
                            } else {
                                tab.style.marginTop = 65 + "px";
                                tab.style.marginLeft = 90 + "px";
                            }


                            /*var dimtab= document.getElementById(cpuname + "pcichannal" +i);
                            if(dimtab != null){
							console.log("pcichannal.offsetTop" + dimtab.offsetTop);
							console.log("pcichannal.offsetLeft" + dimtab.offsetLeft);

							var top= 32 + "px";
							var left= dimtab.offsetLeft - 200 + "px";
							tab.style.marginTop = top;
							tab.style.marginLeft =left;
						}*/

						var tr = document.createElement("TR");
						var td = document.createElement("TD");
						tr.setAttribute("index",that.CHANNAL_WISE_PCI[i].split("-")[2]);
						//td.className = "pcitd";

						if(that.CHANNAL_WISE_PCI[i].split("-")[1]==1){
							tr.onmouseover=function(){
								this.style.backgroundColor="Gold";
							}
							tr.onmouseout=function(){
								this.style.backgroundColor="Green";
							}
							tr.className="pcitr";
							tr.onclick=function(){
								//displayDeviceInfo(this,"PCI");
							}
							//td.className = "pcitd";
						} else {
							//td.className = "dimmtd1";
							tr.className="";
						}
						var lbl = document.createElement("LABEL");
						lbl.className = "pcilbl";
						lbl.innerHTML = that.CHANNAL_WISE_PCI[i].split("-")[1];

						td.appendChild(lbl);
						tr.appendChild(td);
						tab.appendChild(tr);

						divpci.appendChild(tab);
						//console.log(divpci);
						$('#iddivSystemInventory').append(divpci);
					}

				}

			}
            },
            add: function(model, collection, options) {
                var itemView = new SystemInventoryInfoItemView({
                    model: model
                });
                itemView.$el.appendTo(".table-body");
                itemView.render();
                this.$('.footable').trigger('footable_redraw');
            },

        reset: function(model, collection, options) {

            this.$('.footable').trigger('footable_redraw');
        },

        removeModel: function(model, collection, options) {

            this.$('.footable').trigger('footable_redraw');
        },

        change: function(model, collection, options) {

            this.$('.footable').trigger('footable_redraw');
        },

        affect: function(model, collection, options) {

            },

            reload: function(model, value) {

            },
            toggleHelp: function(e) {
                e.preventDefault();
                var help_items = this.$('.help-item').filter(function() {
                    var f = $(this).data('feature');
                    var nf = $(this).data('not-feature');
                    if (f) {
                        return (app.features.indexOf(f) == -1 ? false : true);
                    } else if (nf) {
                        return (app.features.indexOf(nf) == -1 ? true : false);
                    } else {
                        return true;
                    }
                });
                $(".tooltip").hide();
                if (help_items.hasClass('hide')) {
                    help_items.removeClass('hide');
                } else {
                    help_items.addClass('hide');
                }
            },
            serialize: function() {
                return {
                    locale: locale
                };
            }

    });

    return view;

});
