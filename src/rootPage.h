const char rootPage[] PROGMEM = R"===(
<!DOCTYPE html>
<html>
    <head>
        <title>WebServer</title>
    </head>
    <body onload="javascript:start();">
	    <style>
			.loader {
				  position: fixed;
				  left: 43%;
				  top: 35%;
				  border: 16px solid #f3f3f3;
				  border-radius: 50%;
				  border-top: 16px solid #3498db;
				  border-bottom: 16px solid #3498db;
				  width: 120px;
				  height: 120px;
				  -webkit-animation: spin 2s linear infinite;
				  animation: spin 2s linear infinite;
			}

			@-webkit-keyframes spin {
				  0% { -webkit-transform: rotate(0deg); }
				  100% { -webkit-transform: rotate(360deg); }
			}

			@keyframes spin {
				  0% { transform: rotate(0deg); }
				  100% { transform: rotate(360deg); }
			}

      .svgDiv {
          margin-top: 75px; 
      }
  
      .onoffswitch {
          position: fixed;
          top: 30px;
          right: 30px;
          /*position: relative; width: 200px;*/
          -webkit-user-select:none; -moz-user-select:none; -ms-user-select: none;
      }
      .onoffswitch-checkbox {
          position: absolute;
          opacity: 0;
          pointer-events: none;
      }
      .onoffswitch-label {
          display: block; overflow: hidden; cursor: pointer;
          border: 2px solid #999999; border-radius: 50px;
      }
      .onoffswitch-inner {
          display: block; width: 200%; margin-left: -100%;
          transition: margin 0.3s ease-in 0s;
      }
      .onoffswitch-inner:before, .onoffswitch-inner:after {
          display: block; float: left; width: 50%; height: 75px; padding: 0; line-height: 75px;
          font-size: 19px; color: white; font-family: Trebuchet, Arial, sans-serif; font-weight: bold;
          box-sizing: border-box;
      }
      .onoffswitch-inner:before {
          content: "Automatic";
          padding-left: 20px;
          background-color: #34C268; color: #FFFFFF;
      }
      .onoffswitch-inner:after {
          content: "Manual";
          padding-right: 20px;
          background-color: #BF5252; color: #FAFAFA;
          text-align: right;
      }
      .onoffswitch-switch {
          display: block; width: 50px; margin: 12.5px;
          background: #FFFFFF;
          position: absolute; top: 0; bottom: 0;
          right: 121px;
          border: 2px solid #999999; border-radius: 50px;
          transition: all 0.3s ease-in 0s; 
      }
      .onoffswitch-checkbox:checked + .onoffswitch-label .onoffswitch-inner {
          margin-left: 0;
      }
      .onoffswitch-checkbox:checked + .onoffswitch-label .onoffswitch-switch {
          right: 0px; 
      }
			
    </style>
		<div id="loaderDiv" class="loader">
		</div>
        <div id="mainDiv" style="display:none;">
            <div>
                <div class="onoffswitch">
                  <input type="checkbox" name="onoffswitch" class="onoffswitch-checkbox" id="automaticModeSwitch" tabindex="0">
                  <label class="onoffswitch-label" for="automaticModeSwitch">
                      <span class="onoffswitch-inner"></span>
                      <span class="onoffswitch-switch"></span>
                  </label>
                </div>
                <div class="svgDiv">
                 <svg style="display:block; margin:auto;" width="900" height="600" xmlns="http://www.w3.org/2000/svg">
                    <defs>
                        <linearGradient id="lg" x1="0.5" y1="1" x2="0.5" y2="0">
                            <stop offset="0%" stop-opacity="1" stop-color="#31eded"></stop>
                            <stop id="stop1" offset="0%" stop-opacity="1" stop-color="#31eded"></stop>
                            <stop id="stop2" offset="0%" stop-opacity="0" stop-color="#31eded"></stop>
                            <stop offset="100%" stop-opacity="0" stop-color="#31eded"></stop>
                        </linearGradient>
                    </defs>
                    <g>
                        <title>background</title>
                        <rect x="-1" y="-1" width="902" height="602" id="canvas_background" fill="#fff"/>
                        <g id="canvasGrid" display="none">
                            <rect id="svg_1" width="100%" height="100%" x="0" y="0" stroke-width="0" fill="url(#gridpattern)"/>
                        </g>
                    </g>
                    <g>
                        <title>Layer 1</title>
                        <path stroke-width="1.5" d="m597.70501,132.33499c0,48.41697 -94.6918,87.66668 -211.50006,87.66668m211.50006,-87.66668l0,0c0,48.41697 -94.6918,87.66668 -211.50006,87.66668c-116.80822,0 -211.49999,-39.24963 -211.49999,-87.66668m0,0l0,0c0,-48.41692 94.69177,-87.66666 211.49999,-87.66666c116.80826,0 211.50006,39.24974 211.50006,87.66666l0,350.66686c0,48.41692 -94.6918,87.66652 -211.50006,87.66652c-116.80822,0 -211.49999,-39.24961 -211.49999,-87.66652l0,-350.66686z" id="tank" stroke="#000" fill="url(#lg)"/>
                        <text fill="#000000" stroke-width="0" stroke-opacity="null" x="364.60369" y="136.97529" id="level" font-size="30" font-family="Helvetica, Arial, sans-serif" text-anchor="start" xml:space="preserve" stroke="#000">0%</text>
                        <path stroke="null" stroke-width="1.5" stroke-opacity="null" d="m779.28179,234.54428c-12.17374,-2.73934 -19.22633,-14.00137 -18.39444,-24.21792c-0.31165,-13.34472 1.13985,-27.38676 -6.38316,-39.65707c-4.28802,-9.16151 -11.59271,-17.25031 -14.64106,-26.79759c-3.79336,-14.81197 0.89225,-31.59983 15.01024,-41.54368c15.92177,-11.57248 41.34762,-13.08083 58.87432,-3.24492c12.36172,7.22271 21.28097,19.13544 20.94962,32.13504c1.4505,9.71358 -3.06904,18.85004 -8.38578,27.20044c-3.80923,6.7235 -8.25785,13.30278 -11.71531,20.10125c-0.91147,13.93938 -0.38873,28.00662 -2.861,41.8261c-4.11835,10.70852 -19.63882,17.98924 -32.45343,14.19834l0,0.00001zm32.15266,-24.92385c-7.22477,-3.30336 -17.60673,-1.15568 -26.00447,-1.86684c-6.85985,1.16385 -19.04906,-2.38715 -22.69589,2.9395c8.34008,2.76739 18.14141,0.69291 27.02022,1.15076c7.10438,-0.57878 15.27812,0.54886 21.68015,-2.22343l0,0.00001l-0.00001,0zm-0.54108,-6.21597c1.06324,-8.39249 -14.58258,-4.10995 -20.9385,-5.35929c-8.41248,1.44435 -21.80017,-3.09167 -27.09095,3.35656c3.13516,6.47998 17.11712,1.92268 24.4593,3.36042c7.82969,-0.23534 15.95262,0.23804 23.57015,-1.35769l0.00001,0l-0.00001,0zm0.09741,-14.70437c2.06545,-16.34879 13.84994,-29.96122 20.67368,-44.91621c5.61042,-17.90541 -5.50085,-38.36152 -25.61768,-45.87745c-20.02322,-8.4177 -46.70062,-2.05864 -58.01641,14.43567c-10.90284,13.62472 -8.82211,32.13692 1.83405,45.38685c7.60344,11.57975 13.66981,24.59295 12.68162,38.06912c15.78295,0 31.56591,0 47.34888,0c0.36531,-2.36596 0.73049,-4.73207 1.09587,-7.09798l-0.00001,0zm-34.94083,-23.1618c-6.25327,-9.22286 -12.67272,-18.51679 -17.02719,-28.51211c3.91494,-4.29864 5.05382,-9.08688 6.26999,-14.13757c10.98808,-1.83078 -1.39866,10.62154 8.97566,10.90216c9.20878,3.39862 5.16903,-16.49116 12.81986,-9.32184c-5.49203,7.86828 10.78781,15.69574 11.3853,4.43979c-0.78922,-8.49877 9.83997,-6.90645 5.69587,0.80368c0.34204,3.96623 7.95494,5.1033 8.08528,8.29329c-5.48053,10.08366 -10.24429,20.49426 -16.73386,30.14765c-1.46073,-3.73753 5.74518,-13.10501 8.04775,-18.60518c4.44312,-4.75442 8.26114,-17.5672 -3.32179,-15.82068c-6.71912,8.88965 -15.75778,-5.60357 -22.33673,2.91583c-5.58485,1.41309 -14.3686,-8.20431 -15.9887,1.04301c4.20627,10.98344 12.95859,20.34412 17.53795,31.21418c-1.5539,-0.77713 -2.43873,-2.14053 -3.40938,-3.3622l0,0.00001l-0.00001,-0.00002zm-8.77715,-40.25296c-3.0408,-1.25885 1.28167,6.69231 -0.00006,0l0.00006,0zm18.17483,0.93915c-3.94728,-5.66653 -1.83029,6.87527 0,0zm17.61658,0c-3.94728,-5.66653 -1.83032,6.87527 0,0zm-82.0134,-41.87076c-6.97674,-6.00617 -14.68867,-11.64197 -20.75615,-18.26242c15.15787,10.64316 29.03163,22.62963 42.69178,34.64613c3.62737,4.22808 -6.32819,-3.94833 -8.18108,-5.31888c-4.63613,-3.64205 -9.21153,-7.33945 -13.75456,-11.06483l0.00001,0zm107.73492,17.17226c11.7924,-12.46346 22.79497,-25.52963 35.50892,-37.32918c-2.12893,5.08029 -8.9821,11.15923 -13.29112,16.39147c-7.19604,7.594 -13.92244,15.62325 -22.15129,22.43201c-1.02705,0.92084 -2.56068,-1.26024 -0.06639,-1.49431l-0.0001,0l-0.00001,0l-0.00001,0.00001zm-25.54261,-11.82391c4.07437,-9.8853 9.98433,-19.29494 16.57771,-28.14207c-0.15324,4.90794 -7.14601,14.21012 -10.50717,20.29578c-1.90139,2.44892 -3.50515,6.35445 -6.07054,7.84629zm-52.17628,-12.67753c-2.83818,-3.51161 -11.38185,-11.6883 -9.78662,-12.80338c8.2372,7.74704 16.61508,15.74319 22.53229,24.90473c-5.0391,-3.27825 -8.66494,-8.01313 -12.74567,-12.10135l0.00001,0l-0.00001,0zm32.61907,-2.76368c-0.57441,-3.29788 1.20873,-19.04124 2.19933,-8.35118c0.00352,6.69782 0.82772,14.23793 -1.37013,20.33421c-0.94117,-3.91942 -0.71283,-7.99978 -0.8292,-11.98303z" id="state" fill="white"/>
                        <path stroke="null" id="switch" d="m883.16102,424.42278l-38.17296,-10.93868l30.75016,-55.93243c2.74555,-4.99143 -0.62754,-10.65548 -7.56006,-12.63789c-6.93251,-1.98242 -14.76713,0.4602 -17.50289,5.45872l-30.75016,55.93243l-47.85102,-13.71406l31.81728,-55.93243c0.6015,-4.99852 -1.70448,-10.66256 -8.62719,-12.64498c-6.92271,-1.97533 -14.76715,0.4602 -17.50289,5.45872l-30.75017,55.93243l-38.15334,-10.9316l-15.82614,28.78749l21.0819,6.04637l-24.87664,45.24862c-5.98138,10.90328 1.40219,23.24382 16.50271,27.57681l15.9438,4.57372l-8.59946,15.62568c-3.99085,7.25706 0.93153,15.49116 11.00181,18.37982l9.11915,2.61254c10.06049,2.88866 21.45451,-0.67261 25.45518,-7.93675l8.59946,-15.62567l18.21869,5.218c15.10052,4.32591 32.20138,-1.00537 38.20237,-11.90865l24.85703,-45.24862l18.80702,5.38792l15.81634,-28.7875l0.00002,-0.00001z" stroke-opacity="null" stroke-width="1.5" fill="white"/>
                        <path fill-opacity="0" stroke-opacity="0" stroke="#000" id="drop_01" d="m263.08991,195.70486c-3.53228,-1.74386 -6.0641,-7.28493 -5.48872,-12.01247c0.36282,-2.98107 8.01737,-21.07826 8.68585,-20.53544c0.21818,0.17717 2.15975,4.39638 4.3146,9.37603c3.45183,7.97688 3.9179,9.46432 3.9179,12.50399c0,6.16129 -3.75307,11.20808 -8.2594,11.10651c-1.31501,-0.02964 -2.74161,-0.22702 -3.17022,-0.43862l0,0zm3.53895,-3.0989c0.11692,-0.79767 -0.1562,-1.18669 -0.83315,-1.18669c-1.54283,0 -3.683,-2.56733 -4.16114,-4.99168c-0.51005,-2.58613 -1.86242,-2.89369 -2.06217,-0.469c-0.38485,4.67153 6.40811,11.07065 7.05646,6.64736l0,0z" stroke-width="1.5" fill="#56ffff"/>
                        <path fill-opacity="0" stroke-opacity="0" stroke="#000" id="drop_02" d="m237.08991,239.70486c-3.53228,-1.74386 -6.0641,-7.28493 -5.48872,-12.01247c0.36282,-2.98107 8.01737,-21.07826 8.68585,-20.53545c0.21819,0.17717 2.15976,4.39638 4.3146,9.37604c3.45184,7.97687 3.9179,9.46431 3.9179,12.50398c0,6.16129 -3.75307,11.20808 -8.2594,11.10652c-1.31502,-0.02964 -2.74162,-0.22702 -3.17023,-0.43862l0,0zm3.53895,-3.0989c0.11692,-0.79767 -0.1562,-1.18669 -0.83315,-1.18669c-1.54283,0 -3.683,-2.56733 -4.16114,-4.99168c-0.51005,-2.58612 -1.86242,-2.89369 -2.06218,-0.46899c-0.38485,4.67152 6.40811,11.07064 7.05647,6.64736l0,0z" stroke-width="1.5" fill="#56ffff"/>
                        <path fill-opacity="0" stroke-opacity="0" stroke="#000" id="drop_03" d="m293.08991,236.70486c-3.53228,-1.74386 -6.0641,-7.28493 -5.48872,-12.01247c0.36282,-2.98107 8.01737,-21.07826 8.68585,-20.53545c0.21819,0.17717 2.15976,4.39638 4.3146,9.37604c3.45184,7.97687 3.9179,9.46431 3.9179,12.50398c0,6.16129 -3.75307,11.20808 -8.2594,11.10652c-1.31502,-0.02964 -2.74162,-0.22702 -3.17023,-0.43862l0,0zm3.53895,-3.0989c0.11692,-0.79767 -0.1562,-1.18669 -0.83315,-1.18669c-1.54283,0 -3.683,-2.56733 -4.16114,-4.99168c-0.51005,-2.58612 -1.86242,-2.89369 -2.06218,-0.46899c-0.38485,4.67152 6.40811,11.07064 7.05647,6.64736l0,0z" stroke-width="1.5" fill="#56ffff"/>
                        <path fill-opacity="0" stroke-opacity="0" stroke="#000" id="drop_04" d="m324.08991,270.70486c-3.53228,-1.74386 -6.0641,-7.28493 -5.48872,-12.01247c0.36282,-2.98107 8.01737,-21.07826 8.68585,-20.53545c0.21819,0.17717 2.15976,4.39638 4.3146,9.37604c3.45184,7.97687 3.9179,9.46431 3.9179,12.50398c0,6.16129 -3.75307,11.20808 -8.2594,11.10652c-1.31502,-0.02964 -2.74162,-0.22702 -3.17023,-0.43862l0,0zm3.53895,-3.0989c0.11692,-0.79767 -0.1562,-1.18669 -0.83315,-1.18669c-1.54283,0 -3.683,-2.56733 -4.16114,-4.99168c-0.51005,-2.58612 -1.86242,-2.89369 -2.06218,-0.46899c-0.38485,4.67152 6.40811,11.07064 7.05647,6.64736l0,0z" stroke-width="1.5" fill="#56ffff"/>
                        <path fill-opacity="0" stroke-opacity="0" stroke="#000" id="drop_05" d="m270.08991,278.70486c-3.53228,-1.74386 -6.0641,-7.28493 -5.48872,-12.01247c0.36282,-2.98107 8.01737,-21.07826 8.68585,-20.53545c0.21819,0.17717 2.15976,4.39638 4.3146,9.37604c3.45184,7.97687 3.9179,9.46431 3.9179,12.50398c0,6.16129 -3.75307,11.20808 -8.2594,11.10652c-1.31502,-0.02964 -2.74162,-0.22702 -3.17023,-0.43862l0,0zm3.53895,-3.0989c0.11692,-0.79767 -0.1562,-1.18669 -0.83315,-1.18669c-1.54283,0 -3.683,-2.56733 -4.16114,-4.99168c-0.51005,-2.58612 -1.86242,-2.89369 -2.06218,-0.46899c-0.38485,4.67152 6.40811,11.07064 7.05647,6.64736l0,0z" stroke-width="1.5" fill="#56ffff"/>
                        <path fill-opacity="0" stroke-opacity="0" stroke="#000" id="drop_06" d="m233.08991,301.70486c-3.53228,-1.74386 -6.0641,-7.28493 -5.48872,-12.01247c0.36282,-2.98107 8.01737,-21.07826 8.68585,-20.53545c0.21819,0.17717 2.15976,4.39638 4.3146,9.37604c3.45184,7.97687 3.9179,9.46431 3.9179,12.50398c0,6.16129 -3.75307,11.20808 -8.2594,11.10652c-1.31502,-0.02964 -2.74162,-0.22702 -3.17023,-0.43862l0,0zm3.53895,-3.0989c0.11692,-0.79767 -0.1562,-1.18669 -0.83315,-1.18669c-1.54283,0 -3.683,-2.56733 -4.16114,-4.99168c-0.51005,-2.58612 -1.86242,-2.89369 -2.06218,-0.46899c-0.38485,4.67152 6.40811,11.07064 7.05647,6.64736l0,0z" stroke-width="1.5" fill="#56ffff"/>
                        <path fill-opacity="0" stroke-opacity="0" stroke="#000" id="drop_07" d="m301.08991,313.70486c-3.53228,-1.74386 -6.0641,-7.28493 -5.48872,-12.01247c0.36282,-2.98107 8.01737,-21.07826 8.68585,-20.53545c0.21819,0.17717 2.15976,4.39638 4.3146,9.37604c3.45184,7.97687 3.9179,9.46431 3.9179,12.50398c0,6.16129 -3.75307,11.20808 -8.2594,11.10652c-1.31502,-0.02964 -2.74162,-0.22702 -3.17023,-0.43862l0,0zm3.53895,-3.0989c0.11692,-0.79767 -0.1562,-1.18669 -0.83315,-1.18669c-1.54283,0 -3.683,-2.56733 -4.16114,-4.99168c-0.51005,-2.58612 -1.86242,-2.89369 -2.06218,-0.46899c-0.38485,4.67152 6.40811,11.07064 7.05647,6.64736l0,0z" stroke-width="1.5" fill="#56ffff"/>
                        <path fill-opacity="0" stroke-opacity="0" stroke="#000" id="drop_08" d="m265.08991,341.70486c-3.53228,-1.74386 -6.0641,-7.28493 -5.48872,-12.01247c0.36282,-2.98107 8.01737,-21.07826 8.68585,-20.53545c0.21819,0.17717 2.15976,4.39638 4.3146,9.37604c3.45184,7.97687 3.9179,9.46431 3.9179,12.50398c0,6.16129 -3.75307,11.20808 -8.2594,11.10652c-1.31502,-0.02964 -2.74162,-0.22702 -3.17023,-0.43862l0,0zm3.53895,-3.0989c0.11692,-0.79767 -0.1562,-1.18669 -0.83315,-1.18669c-1.54283,0 -3.683,-2.56733 -4.16114,-4.99168c-0.51005,-2.58612 -1.86242,-2.89369 -2.06218,-0.46899c-0.38485,4.67152 6.40811,11.07064 7.05647,6.64736l0,0z" stroke-width="1.5" fill="#56ffff"/>
                        <path id="pipe" d="m-99.135,136.81287l59.16673,-30l236.66601,0c32.67708,0 59.16725,13.43144 59.16725,30c0,16.56838 -26.49017,30 -59.16725,30l-236.66601,0l-59.16673,-30z" fill-opacity="null" stroke-opacity="null" stroke-width="1.5" stroke="#000" fill="white"/>
                        <ellipse ry="27" rx="30" id="pipeend" cy="136" cx="226" fill-opacity="null" stroke-opacity="null" stroke-width="1.5" stroke="#000" fill="white"/>
                    </g>
                </svg>
              </div>
            </div>
        </div>
        <script>
            var isMotorRunning = "false";
            var isAnimationRunning = false;
            var count = 0;
            var stopAnimate = false;
            const switchId = document.getElementById("switch");
            const stateId = document.getElementById("state");
            const automaticSwitch = document.getElementById("automaticModeSwitch");
            const drops = ['drop_01', 'drop_02', 'drop_03', 'drop_04', 'drop_05', 'drop_06', 'drop_07', 'drop_08'];
            let previousAutomaticModeState = false;
            function automaticModeChanged(changedMode){
               if(changedMode != "true" && changedMode != "false"){
                 console.log("Only Two Commands Are Accepted");
                 return;
               }
               try {
                    const params = "automaticMode="+changedMode;
                    var xhttp = new XMLHttpRequest();
                    xhttp.onerror = function() {
                        console.log('Network Error');
                    };
                    xhttp.onreadystatechange = function() {
                        if (this.readyState == 4) {
                          if(this.status == 200) {
                             if(this.responseText === "true"){
                               automaticSwitch.checked = true;
                             } else automaticSwitch.checked = false;
                         } else {
                           automaticSwitch.checked = previousAutomaticModeState; 
                         }
                       }
                    };
                    xhttp.open("POST", "updateAutomaticMode", true);
                    xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
                    xhttp.send(params);
                } catch (err) {
                    automaticSwitch.checked = previousAutomaticModeState; //If there is any error then will restore the previous value;
                    console.log("The error is : " + err);
                }
            }
            function startPipe() {
                stateId.setAttribute('fill', 'green');
                switchId.setAttribute('fill', 'red');
                isMotorRunning = true;
                stopAnimate = false;
                if(!isAnimationRunning){
                  setTimeout(animate, 100); 
                }
                document.getElementById('pipe').setAttribute('fill', '#31eded');
                document.getElementById('pipeend').setAttribute('fill', '#31eded');
                isAnimationRunning=true;
            }
            function stopPipe() {
                stateId.setAttribute('fill', 'white');
                switchId.setAttribute('fill', 'green');
                isMotorRunning = false;
                stopAnimate = true;
                document.getElementById('pipe').setAttribute('fill', 'white');
                document.getElementById('pipeend').setAttribute('fill', 'white');
                isAnimationRunning=false;
            }
            function animate() {
                if (count >= 8) {
                    count = 0;
                    clearDrop();
                    if (!stopAnimate)
                        setTimeout(animate, 100);
                } else {
                    document.getElementById(drops[count]).setAttribute('fill-opacity', '1');
                    document.getElementById(drops[count]).setAttribute('stroke-opacity', '0.3');
                    count++;
                    setTimeout(animate, 200);
                }
            }
            function clearDrop() {
                let drops = ['drop_01', 'drop_02', 'drop_03', 'drop_04', 'drop_05', 'drop_06', 'drop_07', 'drop_08'];
                for (var i = 0; i < drops.length; i++) {
                    document.getElementById(drops[i]).setAttribute('fill-opacity', '0');
                    document.getElementById(drops[i]).setAttribute('stroke-opacity', '0');
                }
            }
            function updateValue(command) {
              try{
                    const updates = command.split("/");
                    const stop1 = document.getElementById("stop1");
                    const stop2 = document.getElementById("stop2");
                    const level = document.getElementById("level");
                    const percentage = updates[1] + "%";
                    level.innerHTML = percentage;
                    stop1.setAttribute("offset", percentage);
                    stop2.setAttribute("offset", percentage);
                    if (updates[0] === "true") {
                        startPipe();
                    } else {
                        stopPipe();
                    }
                    if(updates[3] === "true"){
                      automaticSwitch.checked = true;
                    } else {
                      automaticSwitch.checked = false;
                    }
                    console.log(new Date().toLocaleString() + " % :- " + updates[1] + " H :- " + updates[2]);
              } catch(err){
                console.log("The error is :- "+err);
              }
              
            }
            function start() {
                var Socket;
                Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
                Socket.onmessage = function(evt) {
                    let command = evt.data;
                    command = command.replace('\0', '');
                    updateValue(command);
                };
				        try {
                    var xhttp = new XMLHttpRequest();
                    xhttp.onerror = function() {
                        console.log('Network Error');
                    };
                    xhttp.onreadystatechange = function() {
                        if (this.readyState == 4 && this.status == 200) {
                            updateValue(this.responseText);
						                document.getElementById('mainDiv').style.display="";
						                document.getElementById('loaderDiv').style.display="none";
                        }
                    };
                    xhttp.open("GET", "initialValue", true);
                    xhttp.send();
                } catch (err) {
                    console.log("The error is : " + err);
                }
            }
            automaticSwitch.addEventListener( 'focus', function() {
               previousAutomaticModeState = this.checked;
            });
            automaticSwitch.addEventListener( 'change', function() {
              if(this.checked) {
                  automaticModeChanged("true");
                  console.log("Automatic Mode");
              } else {
                  automaticModeChanged("false");
                  console.log("Manual Mode");
              }
            });
            
            switchId.addEventListener("click", function() {
                let command = stateId.getAttribute('fill') == 'green' ? 'motorOff' : 'motorOn';
                try {
                    var xhttp = new XMLHttpRequest();
                    xhttp.onerror = function() {
                        console.log('Network Error');
                    };
                    xhttp.onreadystatechange = function() {
                        if (this.readyState == 4 && this.status == 200) {
                            if (command == 'motorOn') {
                                /*
                  								startPipe();
                  								Uncomment this if you want to terminate the web sockets
                								*/
                                console.log('Motor Started');
                            } else {
                                /*
                  								stopPipe();
                  								Uncomment this if you want to terminate the web sockets
                								*/
                                console.log('Motor Stopped');
                            }
                        }
                    };
                    xhttp.open("GET", command, true);
                    xhttp.send();
                } catch (err) {
                    console.log("The error is : " + err);
                }
            });
        </script>
    </body>
</html>
)===";
