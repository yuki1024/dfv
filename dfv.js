var json;

function import_button_handler(){
	document.getElementById('import_button').addEventListener('change', e => {
		let files = e.target.files;
		for (let i=0; i<files.length; i++){
			let reader = new FileReader();
			reader.readAsText(files[i]);
			reader.onload = function(e){ //proceed asynchronously
				json = JSON.parse(reader.result);
				createMainBody();
			}
		}
	});
}

document.getElementById('export_button').addEventListener('click', e => {
	const input_text = JSON.stringify(json, null, '\t');
	const blob = new Blob([input_text], {type: 'text/plain'});
	const a = document.createElement('a');
	a.href =  URL.createObjectURL(blob);
	a.download = 'dfv.json';
	a.click();
});

var blank_core_num = 0;
document.getElementById('add_blank_button').addEventListener('click', e => {
	let count = 0;
	blank_core_num--;
	Object.keys(json).forEach(function(mn){
		let s = '<div class="cr ' + blank_core_num + ' dd" style="background-color: grey"></div>';
		document.querySelector('#grid'+count).insertAdjacentHTML('beforeend', s);

   	//add to json
		json[mn][blank_core_num] = {};
		json[mn][blank_core_num]['position'] = {};
		json[mn][blank_core_num]['position']['x'] = 0;
		json[mn][blank_core_num]['position']['y'] = 0;
		json[mn][blank_core_num]['up'] = NaN;

   	count++;
	});

	let elements = document.getElementsByClassName(blank_core_num);
	for(let i = 0; i < elements.length; i++) {
		elements[i].addEventListener("mousedown", mdown, false);
		elements[i].addEventListener("touchstart", mdown, false);
	}

});

var sel_el_name = '';
document.getElementById('hrv_button').addEventListener('click', e => {
	let current_rv = '';
	Object.keys(json).forEach(function(mn){
		if(json[mn][sel_el_name]['rv'] === '') {
			current_rv = 'h';
		} else if(json[mn][sel_el_name]['rv'] === 'v'){
			current_rv = 'hv';
		} else if(json[mn][sel_el_name]['rv'] === 'h'){
			current_rv = '';
		} else if(json[mn][sel_el_name]['rv'] === 'hv'){
			current_rv = 'v';
		}
		json[mn][sel_el_name]['rv'] = current_rv;
	});
	let elements = document.getElementsByClassName(sel_el_name);
	for(let i = 0; i < elements.length; i++) {
		let temp_color_s = elements[i].children[2].style.backgroundColor;
		elements[i].children[2].style.backgroundColor = elements[i].children[3].style.backgroundColor;
		elements[i].children[3].style.backgroundColor = temp_color_s;
		elements[i].children[5].textContent = current_rv;
	}
});

document.getElementById('vrv_button').addEventListener('click', e => {
	let current_rv = '';
	Object.keys(json).forEach(function(mn){
		if(json[mn][sel_el_name]['rv'] === '') {
			current_rv = 'v';
		} else if(json[mn][sel_el_name]['rv'] === 'v'){
			current_rv = '';
		} else if(json[mn][sel_el_name]['rv'] === 'h'){
			current_rv = 'hv';
		} else if(json[mn][sel_el_name]['rv'] === 'hv'){
			current_rv = 'h';
		}
		json[mn][sel_el_name]['rv'] = current_rv;
	});
	let elements = document.getElementsByClassName(sel_el_name);
	for(let i = 0; i < elements.length; i++) {
		let temp_color_s = elements[i].children[0].style.backgroundColor;
		elements[i].children[0].style.backgroundColor = elements[i].children[1].style.backgroundColor;
		elements[i].children[1].style.backgroundColor = temp_color_s;
		elements[i].children[5].textContent = current_rv;
	}
});

document.getElementById('cmt_button').addEventListener('click', e => {
	let cmt = window.prompt('Input a comment which is put on the right corner of selected CHA box', '');
	let elements = document.getElementsByClassName(sel_el_name);
	Object.keys(json).forEach(function(mn){
		json[mn][sel_el_name]['cmt'] = cmt;
	});
	for(let i = 0; i < elements.length; i++) {
		elements[i].children[6].textContent = cmt;
	}
});

function createMainBody(){
	let s = '';

	for(let i=0; i<Object.keys(json).length; i++){
		s+= '<div class="grid" id="grid'+i+'" style="top: ' + ((Math.floor(i/2))*600+100) + 'px; left: ' + ((i%2)*800+100) + 'px;"></div>';
	}

	document.querySelector('#all_body').insertAdjacentHTML('beforeend', s);

	let count = 0;
	Object.keys(json).forEach(function(mn){
		s = '';
		Object.keys(json[mn]).forEach(function(k){
			s+= '<div class="cr ' + k + ' dd">'
				+ '<div class="incr up"></div>'
				+ '<div class="incr dn"></div>'
				+ '<div class="incr lt"></div>'
				+ '<div class="incr rt"></div>'
				+ '</div>';
		});
		document.querySelector('#grid'+count).insertAdjacentHTML('beforeend', s);
		count++;
	});

	count = 0;
	Object.keys(json).forEach(function(mn){
    let grid = document.getElementById("grid"+count);

    let vals = [];
		Object.keys(json[mn]).forEach(function(k){
				if (json[mn][k]['up'] !== null) {
					vals.push(json[mn][k]['up']);
					vals.push(json[mn][k]['dn']);
					vals.push(json[mn][k]['lt']);
					vals.push(json[mn][k]['rt']);
				}
		});
   	let cmax = Math.max(...vals);
   	//let cmin = Math.min(...vals);
   	let cmin = 0

   	let gc_count = 0;
		Object.keys(json[mn]).forEach(function(k){

   		let num_up = parseFloat(json[mn][k]['up']);
   		let num_dn = parseFloat(json[mn][k]['dn']);
   		let num_lt = parseFloat(json[mn][k]['lt']);
   		let num_rt = parseFloat(json[mn][k]['rt']);
   		if (num_up > 0) {

	    	grid.children[gc_count].insertAdjacentHTML('beforeend', '<div class="crtext">'+k+'</div>');
	    	grid.children[gc_count].insertAdjacentHTML('beforeend', '<div class="crtext_rv"></div>');
	    	let cmt = '';
	    	if(json[mn][k].hasOwnProperty('cmt')) {
	    		cmt = json[mn][k]['cmt'];
	    	}
	    	grid.children[gc_count].insertAdjacentHTML('beforeend', '<div class="crtext_cmt">'+cmt+'</div>');

		    if (json[mn][k]['rv'] === '') {
			    grid.children[gc_count].children[1].style.backgroundColor = coloring(num_up,cmin,cmax);
			    grid.children[gc_count].children[0].style.backgroundColor = coloring(num_dn,cmin,cmax);
			    grid.children[gc_count].children[3].style.backgroundColor = coloring(num_lt,cmin,cmax);
			    grid.children[gc_count].children[2].style.backgroundColor = coloring(num_rt,cmin,cmax);
					grid.children[gc_count].children[5].textContent = '';
		    } else if (json[mn][k]['rv'] === 'h') {
			    grid.children[gc_count].children[1].style.backgroundColor = coloring(num_up,cmin,cmax);
			    grid.children[gc_count].children[0].style.backgroundColor = coloring(num_dn,cmin,cmax);
			    grid.children[gc_count].children[2].style.backgroundColor = coloring(num_lt,cmin,cmax);
			    grid.children[gc_count].children[3].style.backgroundColor = coloring(num_rt,cmin,cmax);
					grid.children[gc_count].children[5].textContent = 'h';
		    } else if (json[mn][k]['rv'] === 'v') {
			    grid.children[gc_count].children[0].style.backgroundColor = coloring(num_up,cmin,cmax);
			    grid.children[gc_count].children[1].style.backgroundColor = coloring(num_dn,cmin,cmax);
			    grid.children[gc_count].children[3].style.backgroundColor = coloring(num_lt,cmin,cmax);
			    grid.children[gc_count].children[2].style.backgroundColor = coloring(num_rt,cmin,cmax);
					grid.children[gc_count].children[5].textContent = 'v';
		    } else if (json[mn][k]['rv'] === 'hv') {
			    grid.children[gc_count].children[0].style.backgroundColor = coloring(num_up,cmin,cmax);
			    grid.children[gc_count].children[1].style.backgroundColor = coloring(num_dn,cmin,cmax);
			    grid.children[gc_count].children[2].style.backgroundColor = coloring(num_lt,cmin,cmax);
			    grid.children[gc_count].children[3].style.backgroundColor = coloring(num_rt,cmin,cmax);
					grid.children[gc_count].children[5].textContent = 'hv';
		    }

   		} else {
		    grid.children[gc_count].style.backgroundColor = 'grey';
		    if (num_up === 0) {
	   			//blank CHA
		    } else {
		    	//IMC
			    grid.children[gc_count].children[0].style.backgroundColor = 'grey';
			    grid.children[gc_count].children[1].style.backgroundColor = 'grey';
			    grid.children[gc_count].children[2].style.backgroundColor = 'grey';
			    grid.children[gc_count].children[3].style.backgroundColor = 'grey';
		    }
   		}
    	gc_count++;
		});

   	grid.insertAdjacentHTML('beforeend', '<div class="gridtitle">'+mn+'</div>');

		count++;
	});

  //init positions
	Object.keys(json[Object.keys(json)[0]]).forEach(function(k){
    let same_core_list = document.getElementsByClassName(k);
    for(let i=0; i<same_core_list.length; i++){
     	let init_y = json[Object.keys(json)[0]][k]['position']['y'];
     	let init_x = json[Object.keys(json)[0]][k]['position']['x'];
     	same_core_list[i].style.top = init_y + "px";
     	same_core_list[i].style.left = init_x + "px";
     }
	});

	//for drag&drop
	let elements = document.getElementsByClassName("dd");
	for(let i = 0; i < elements.length; i++) {
		elements[i].addEventListener("mousedown", mdown, false);
		elements[i].addEventListener("touchstart", mdown, false);
	}

}

function coloring(num,cmin,cmax){
	//pink rgb(255,0,255)
	//cyan rgb(0,255,255)

	let range = cmax-cmin;
	let red = Math.floor(255.0*(num-cmin)/range);
	let green = Math.floor(255.0*(cmax-num)/range);

	let color_s = 'rgb(' + String(red) + ',' + String(green) + ',255)';

	//return 'rgb(255,0,0)';
	return color_s;
}

//-------------------------------------------------------------
//-------------------------------------------------------------

var x;
var y;

function mdown(e) {
	sel_el_name = this.classList[1];
	this.classList.add("drag");

	//タッチイベントとマウスのイベントの差異を吸収
	if(e.type === "mousedown") {
		let event = e;
	} else {
		let event = e.changedTouches[0];
	}

	//要素内の相対座標を取得
	x = event.pageX - this.offsetLeft;
	y = event.pageY - this.offsetTop;

	//ムーブイベントにコールバック
	document.body.addEventListener("mousemove", mmove, false);
	document.body.addEventListener("touchmove", mmove, false);
}

function mmove(e) {
	var drag = document.getElementsByClassName("drag")[0];

	if(e.type === "mousemove") {
		var event = e;
	} else {
		var event = e.changedTouches[0];
	}

	//フリックしたときに画面を動かさないようにデフォルト動作を抑制
	e.preventDefault();

	//マウスが動いた場所に要素を動かす
	drag.style.top = event.pageY - y + "px";
	drag.style.left = event.pageX - x + "px";

	//-------------------------------------------------------------
	let same_core_list = document.getElementsByClassName((drag.className.split(' '))[1]);
	for(let i=0; i<same_core_list.length; i++){
		same_core_list[i].style.top = event.pageY - y + "px";
		same_core_list[i].style.left = event.pageX - x + "px";
	}

	Object.keys(json).forEach(function(mn){
		json[mn][((drag.className.split(' '))[1])]['position']['y'] = event.pageY - y;
		json[mn][((drag.className.split(' '))[1])]['position']['x'] = event.pageX - x;
	});

	//-------------------------------------------------------------

	//マウスボタンが離されたとき、またはカーソルが外れたとき発火
	drag.addEventListener("mouseup", mup, false);
	document.body.addEventListener("mouseleave", mup, false);
	drag.addEventListener("touchend", mup, false);
	document.body.addEventListener("touchleave", mup, false);
}

function mup(e) {
	var drag = document.getElementsByClassName("drag")[0];

	//ムーブイベントハンドラの消去
	document.body.removeEventListener("mousemove", mmove, false);
	drag.removeEventListener("mouseup", mup, false);
	document.body.removeEventListener("touchmove", mmove, false);
	drag.removeEventListener("touchend", mup, false);

	drag.classList.remove("drag");
}


//----------------------main---------------------
import_button_handler();



