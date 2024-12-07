import{q as y,c as H,bV as ve,ad as z,J as ge,bO as me,z as le,r as G,bT as C,aN as x,b as be,M as D,ay as Se,V as se,D as ie,P as ye,d as O,a4 as B,bo as Me,Q as q,bt as Te,a1 as we,H as He}from"./index-CXUYW_Aq.js";const Ee=`accept acceptcharset accesskey action allowfullscreen allowtransparency
alt async autocomplete autofocus autoplay capture cellpadding cellspacing challenge
charset checked classid classname colspan cols content contenteditable contextmenu
controls coords crossorigin data datetime default defer dir disabled download draggable
enctype form formaction formenctype formmethod formnovalidate formtarget frameborder
headers height hidden high href hreflang htmlfor for httpequiv icon id inputmode integrity
is keyparams keytype kind label lang list loop low manifest marginheight marginwidth max maxlength media
mediagroup method min minlength multiple muted name novalidate nonce open
optimum pattern placeholder poster preload radiogroup readonly rel required
reversed role rowspan rows sandbox scope scoped scrolling seamless selected
shape size sizes span spellcheck src srcdoc srclang srcset start step style
summary tabindex target title type usemap value width wmode wrap`,Re=`onCopy onCut onPaste onCompositionend onCompositionstart onCompositionupdate onKeydown
    onKeypress onKeyup onFocus onBlur onChange onInput onSubmit onClick onContextmenu onDoubleclick onDblclick
    onDrag onDragend onDragenter onDragexit onDragleave onDragover onDragstart onDrop onMousedown
    onMouseenter onMouseleave onMousemove onMouseout onMouseover onMouseup onSelect onTouchcancel
    onTouchend onTouchmove onTouchstart onTouchstartPassive onTouchmovePassive onScroll onWheel onAbort onCanplay onCanplaythrough
    onDurationchange onEmptied onEncrypted onEnded onError onLoadeddata onLoadedmetadata
    onLoadstart onPause onPlay onPlaying onProgress onRatechange onSeeked onSeeking onStalled onSuspend onTimeupdate onVolumechange onWaiting onLoad onError`,ee=`${Ee} ${Re}`.split(/[\s\n]+/),xe="aria-",Le="data-";function te(e,t){return e.indexOf(t)===0}function ze(e){let t=arguments.length>1&&arguments[1]!==void 0?arguments[1]:!1,s;t===!1?s={aria:!0,data:!0,attr:!0}:t===!0?s={aria:!0}:s=y({},t);const n={};return Object.keys(e).forEach(l=>{(s.aria&&(l==="role"||te(l,xe))||s.data&&te(l,Le)||s.attr&&(ee.includes(l)||ee.includes(l.toLowerCase())))&&(n[l]=e[l])}),n}function oe(){const e=t=>{e.current=t};return e}const U=(e,t)=>{let{height:s,offset:n,prefixCls:l,onInnerResize:i}=e,{slots:h}=t;var a;let d={},r={display:"flex",flexDirection:"column"};return n!==void 0&&(d={height:`${s}px`,position:"relative",overflow:"hidden"},r=y(y({},r),{transform:`translateY(${n}px)`,position:"absolute",left:0,right:0,top:0})),H("div",{style:d},[H(ve,{onResize:p=>{let{offsetHeight:f}=p;f&&i&&i()}},{default:()=>[H("div",{style:r,class:z({[`${l}-holder-inner`]:l})},[(a=h.default)===null||a===void 0?void 0:a.call(h)])]})])};U.displayName="Filter";U.inheritAttrs=!1;U.props={prefixCls:String,height:Number,offset:Number,onInnerResize:Function};const re=(e,t)=>{let{setRef:s}=e,{slots:n}=t;var l;const i=ge((l=n.default)===null||l===void 0?void 0:l.call(n));return i&&i.length?me(i[0],{ref:s}):i};re.props={setRef:{type:Function,default:()=>{}}};const Ce=20;function ne(e){return"touches"in e?e.touches[0].pageY:e.pageY}const De=le({compatConfig:{MODE:3},name:"ScrollBar",inheritAttrs:!1,props:{prefixCls:String,scrollTop:Number,scrollHeight:Number,height:Number,count:Number,onScroll:{type:Function},onStartMove:{type:Function},onStopMove:{type:Function}},setup(){return{moveRaf:null,scrollbarRef:oe(),thumbRef:oe(),visibleTimeout:null,state:G({dragging:!1,pageY:null,startTop:null,visible:!1})}},watch:{scrollTop:{handler(){this.delayHidden()},flush:"post"}},mounted(){var e,t;(e=this.scrollbarRef.current)===null||e===void 0||e.addEventListener("touchstart",this.onScrollbarTouchStart,C?{passive:!1}:!1),(t=this.thumbRef.current)===null||t===void 0||t.addEventListener("touchstart",this.onMouseDown,C?{passive:!1}:!1)},beforeUnmount(){this.removeEvents(),clearTimeout(this.visibleTimeout)},methods:{delayHidden(){clearTimeout(this.visibleTimeout),this.state.visible=!0,this.visibleTimeout=setTimeout(()=>{this.state.visible=!1},2e3)},onScrollbarTouchStart(e){e.preventDefault()},onContainerMouseDown(e){e.stopPropagation(),e.preventDefault()},patchEvents(){window.addEventListener("mousemove",this.onMouseMove),window.addEventListener("mouseup",this.onMouseUp),this.thumbRef.current.addEventListener("touchmove",this.onMouseMove,C?{passive:!1}:!1),this.thumbRef.current.addEventListener("touchend",this.onMouseUp)},removeEvents(){window.removeEventListener("mousemove",this.onMouseMove),window.removeEventListener("mouseup",this.onMouseUp),this.scrollbarRef.current.removeEventListener("touchstart",this.onScrollbarTouchStart,C?{passive:!1}:!1),this.thumbRef.current&&(this.thumbRef.current.removeEventListener("touchstart",this.onMouseDown,C?{passive:!1}:!1),this.thumbRef.current.removeEventListener("touchmove",this.onMouseMove,C?{passive:!1}:!1),this.thumbRef.current.removeEventListener("touchend",this.onMouseUp)),x.cancel(this.moveRaf)},onMouseDown(e){const{onStartMove:t}=this.$props;y(this.state,{dragging:!0,pageY:ne(e),startTop:this.getTop()}),t(),this.patchEvents(),e.stopPropagation(),e.preventDefault()},onMouseMove(e){const{dragging:t,pageY:s,startTop:n}=this.state,{onScroll:l}=this.$props;if(x.cancel(this.moveRaf),t){const i=ne(e)-s,h=n+i,a=this.getEnableScrollRange(),d=this.getEnableHeightRange(),r=d?h/d:0,p=Math.ceil(r*a);this.moveRaf=x(()=>{l(p)})}},onMouseUp(){const{onStopMove:e}=this.$props;this.state.dragging=!1,e(),this.removeEvents()},getSpinHeight(){const{height:e,scrollHeight:t}=this.$props;let s=e/t*100;return s=Math.max(s,Ce),s=Math.min(s,e/2),Math.floor(s)},getEnableScrollRange(){const{scrollHeight:e,height:t}=this.$props;return e-t||0},getEnableHeightRange(){const{height:e}=this.$props,t=this.getSpinHeight();return e-t||0},getTop(){const{scrollTop:e}=this.$props,t=this.getEnableScrollRange(),s=this.getEnableHeightRange();return e===0||t===0?0:e/t*s},showScroll(){const{height:e,scrollHeight:t}=this.$props;return t>e}},render(){const{dragging:e,visible:t}=this.state,{prefixCls:s}=this.$props,n=this.getSpinHeight()+"px",l=this.getTop()+"px",i=this.showScroll(),h=i&&t;return H("div",{ref:this.scrollbarRef,class:z(`${s}-scrollbar`,{[`${s}-scrollbar-show`]:i}),style:{width:"8px",top:0,bottom:0,right:0,position:"absolute",display:h?void 0:"none"},onMousedown:this.onContainerMouseDown,onMousemove:this.delayHidden},[H("div",{ref:this.thumbRef,class:z(`${s}-scrollbar-thumb`,{[`${s}-scrollbar-thumb-moving`]:e}),style:{width:"100%",height:n,top:l,left:0,position:"absolute",background:"rgba(0, 0, 0, 0.5)",borderRadius:"99px",cursor:"pointer",userSelect:"none"},onMousedown:this.onMouseDown},null)])}});function Fe(e,t,s,n){const l=new Map,i=new Map,h=be(Symbol("update"));D(e,()=>{h.value=Symbol("update")});let a;function d(){x.cancel(a)}function r(){d(),a=x(()=>{l.forEach((f,c)=>{if(f&&f.offsetParent){const{offsetHeight:v}=f;i.get(c)!==v&&(h.value=Symbol("update"),i.set(c,f.offsetHeight))}})})}function p(f,c){const v=t(f);l.get(v),c?(l.set(v,c.$el||c),r()):l.delete(v)}return Se(()=>{d()}),[p,r,i,h]}function Pe(e,t,s,n,l,i,h,a){let d;return r=>{if(r==null){a();return}x.cancel(d);const p=t.value,f=n.itemHeight;if(typeof r=="number")h(r);else if(r&&typeof r=="object"){let c;const{align:v}=r;"index"in r?{index:c}=r:c=p.findIndex(T=>l(T)===r.key);const{offset:m=0}=r,$=(T,F)=>{if(T<0||!e.value)return;const P=e.value.clientHeight;let b=!1,M=F;if(P){const k=F||v;let N=0,E=0,R=0;const Y=Math.min(p.length,c);for(let S=0;S<=Y;S+=1){const V=l(p[S]);E=N;const _=s.get(V);R=E+(_===void 0?f:_),N=R,S===c&&_===void 0&&(b=!0)}const I=e.value.scrollTop;let w=null;switch(k){case"top":w=E-m;break;case"bottom":w=R-P+m;break;default:{const S=I+P;E<I?M="top":R>S&&(M="bottom")}}w!==null&&w!==I&&h(w)}d=x(()=>{b&&i(),$(T-1,M)},2)};$(5)}}}const ke=typeof navigator=="object"&&/Firefox/i.test(navigator.userAgent),ae=(e,t)=>{let s=!1,n=null;function l(){clearTimeout(n),s=!0,n=setTimeout(()=>{s=!1},50)}return function(i){let h=arguments.length>1&&arguments[1]!==void 0?arguments[1]:!1;const a=i<0&&e.value||i>0&&t.value;return h&&a?(clearTimeout(n),s=!1):(!a||s)&&l(),!s&&a}};function Ie(e,t,s,n){let l=0,i=null,h=null,a=!1;const d=ae(t,s);function r(f){if(!e.value)return;x.cancel(i);const{deltaY:c}=f;l+=c,h=c,!d(c)&&(ke||f.preventDefault(),i=x(()=>{n(l*(a?10:1)),l=0}))}function p(f){e.value&&(a=f.detail===h)}return[r,p]}const Oe=14/15;function $e(e,t,s){let n=!1,l=0,i=null,h=null;const a=()=>{i&&(i.removeEventListener("touchmove",d),i.removeEventListener("touchend",r))},d=c=>{if(n){const v=Math.ceil(c.touches[0].pageY);let m=l-v;l=v,s(m)&&c.preventDefault(),clearInterval(h),h=setInterval(()=>{m*=Oe,(!s(m,!0)||Math.abs(m)<=.1)&&clearInterval(h)},16)}},r=()=>{n=!1,a()},p=c=>{a(),c.touches.length===1&&!n&&(n=!0,l=Math.ceil(c.touches[0].pageY),i=c.target,i.addEventListener("touchmove",d,{passive:!1}),i.addEventListener("touchend",r))},f=()=>{};se(()=>{document.addEventListener("touchmove",f,{passive:!1}),D(e,c=>{t.value.removeEventListener("touchstart",p),a(),clearInterval(h),c&&t.value.addEventListener("touchstart",p,{passive:!1})},{immediate:!0})}),ie(()=>{document.removeEventListener("touchmove",f)})}var Ne=function(e,t){var s={};for(var n in e)Object.prototype.hasOwnProperty.call(e,n)&&t.indexOf(n)<0&&(s[n]=e[n]);if(e!=null&&typeof Object.getOwnPropertySymbols=="function")for(var l=0,n=Object.getOwnPropertySymbols(e);l<n.length;l++)t.indexOf(n[l])<0&&Object.prototype.propertyIsEnumerable.call(e,n[l])&&(s[n[l]]=e[n[l]]);return s};const _e=[],Be={overflowY:"auto",overflowAnchor:"none"};function Ye(e,t,s,n,l,i){let{getKey:h}=i;return e.slice(t,s+1).map((a,d)=>{const r=t+d,p=l(a,r,{}),f=h(a);return H(re,{key:f,setRef:c=>n(a,c)},{default:()=>[p]})})}const Ue=le({compatConfig:{MODE:3},name:"List",inheritAttrs:!1,props:{prefixCls:String,data:ye.array,height:Number,itemHeight:Number,fullHeight:{type:Boolean,default:void 0},itemKey:{type:[String,Number,Function],required:!0},component:{type:[String,Object]},virtual:{type:Boolean,default:void 0},children:Function,onScroll:Function,onMousedown:Function,onMouseenter:Function,onVisibleChange:Function},setup(e,t){let{expose:s}=t;const n=O(()=>{const{height:o,itemHeight:u,virtual:g}=e;return!!(g!==!1&&o&&u)}),l=O(()=>{const{height:o,itemHeight:u,data:g}=e;return n.value&&g&&u*g.length>o}),i=G({scrollTop:0,scrollMoving:!1}),h=O(()=>e.data||_e),a=B([]);D(h,()=>{a.value=Me(h.value).slice()},{immediate:!0});const d=B(o=>{});D(()=>e.itemKey,o=>{typeof o=="function"?d.value=o:d.value=u=>u==null?void 0:u[o]},{immediate:!0});const r=B(),p=B(),f=B(),c=o=>d.value(o),v={getKey:c};function m(o){let u;typeof o=="function"?u=o(i.scrollTop):u=o;const g=N(u);r.value&&(r.value.scrollTop=g),i.scrollTop=g}const[$,T,F,P]=Fe(a,c),b=G({scrollHeight:void 0,start:0,end:0,offset:void 0}),M=B(0);se(()=>{q(()=>{var o;M.value=((o=p.value)===null||o===void 0?void 0:o.offsetHeight)||0})}),Te(()=>{q(()=>{var o;M.value=((o=p.value)===null||o===void 0?void 0:o.offsetHeight)||0})}),D([n,a],()=>{n.value||y(b,{scrollHeight:void 0,start:0,end:a.value.length-1,offset:void 0})},{immediate:!0}),D([n,a,M,l],()=>{n.value&&!l.value&&y(b,{scrollHeight:M.value,start:0,end:a.value.length-1,offset:void 0}),r.value&&(i.scrollTop=r.value.scrollTop)},{immediate:!0}),D([l,n,()=>i.scrollTop,a,P,()=>e.height,M],()=>{if(!n.value||!l.value)return;let o=0,u,g,L;const j=a.value.length,he=a.value,Q=i.scrollTop,{itemHeight:Z,height:X}=e,fe=Q+X;for(let A=0;A<j;A+=1){const de=he[A],pe=c(de);let K=F.get(pe);K===void 0&&(K=Z);const W=o+K;u===void 0&&W>=Q&&(u=A,g=o),L===void 0&&W>fe&&(L=A),o=W}u===void 0&&(u=0,g=0,L=Math.ceil(X/Z)),L===void 0&&(L=j-1),L=Math.min(L+1,j),y(b,{scrollHeight:o,start:u,end:L,offset:g})},{immediate:!0});const k=O(()=>b.scrollHeight-e.height);function N(o){let u=o;return Number.isNaN(k.value)||(u=Math.min(u,k.value)),u=Math.max(u,0),u}const E=O(()=>i.scrollTop<=0),R=O(()=>i.scrollTop>=k.value),Y=ae(E,R);function I(o){m(o)}function w(o){var u;const{scrollTop:g}=o.currentTarget;g!==i.scrollTop&&m(g),(u=e.onScroll)===null||u===void 0||u.call(e,o)}const[S,V]=Ie(n,E,R,o=>{m(u=>u+o)});$e(n,r,(o,u)=>Y(o,u)?!1:(S({preventDefault(){},deltaY:o}),!0));function _(o){n.value&&o.preventDefault()}const J=()=>{r.value&&(r.value.removeEventListener("wheel",S,C?{passive:!1}:!1),r.value.removeEventListener("DOMMouseScroll",V),r.value.removeEventListener("MozMousePixelScroll",_))};we(()=>{q(()=>{r.value&&(J(),r.value.addEventListener("wheel",S,C?{passive:!1}:!1),r.value.addEventListener("DOMMouseScroll",V),r.value.addEventListener("MozMousePixelScroll",_))})}),ie(()=>{J()});const ce=Pe(r,a,F,e,c,T,m,()=>{var o;(o=f.value)===null||o===void 0||o.delayHidden()});s({scrollTo:ce});const ue=O(()=>{let o=null;return e.height&&(o=y({[e.fullHeight?"height":"maxHeight"]:e.height+"px"},Be),n.value&&(o.overflowY="hidden",i.scrollMoving&&(o.pointerEvents="none"))),o});return D([()=>b.start,()=>b.end,a],()=>{if(e.onVisibleChange){const o=a.value.slice(b.start,b.end+1);e.onVisibleChange(o,a.value)}},{flush:"post"}),{state:i,mergedData:a,componentStyle:ue,onFallbackScroll:w,onScrollBar:I,componentRef:r,useVirtual:n,calRes:b,collectHeight:T,setInstance:$,sharedConfig:v,scrollBarRef:f,fillerInnerRef:p,delayHideScrollBar:()=>{var o;(o=f.value)===null||o===void 0||o.delayHidden()}}},render(){const e=y(y({},this.$props),this.$attrs),{prefixCls:t="rc-virtual-list",height:s,itemHeight:n,fullHeight:l,data:i,itemKey:h,virtual:a,component:d="div",onScroll:r,children:p=this.$slots.default,style:f,class:c}=e,v=Ne(e,["prefixCls","height","itemHeight","fullHeight","data","itemKey","virtual","component","onScroll","children","style","class"]),m=z(t,c),{scrollTop:$}=this.state,{scrollHeight:T,offset:F,start:P,end:b}=this.calRes,{componentStyle:M,onFallbackScroll:k,onScrollBar:N,useVirtual:E,collectHeight:R,sharedConfig:Y,setInstance:I,mergedData:w,delayHideScrollBar:S}=this;return H("div",He({style:y(y({},f),{position:"relative"}),class:m},v),[H(d,{class:`${t}-holder`,style:M,ref:"componentRef",onScroll:k,onMouseenter:S},{default:()=>[H(U,{prefixCls:t,height:T,offset:F,onInnerResize:R,ref:"fillerInnerRef"},{default:()=>Ye(w,P,b,I,p,Y)})]}),E&&H(De,{ref:"scrollBarRef",prefixCls:t,scrollTop:$,height:s,scrollHeight:T,count:w.length,onScroll:N,onStartMove:()=>{this.state.scrollMoving=!0},onStopMove:()=>{this.state.scrollMoving=!1}},null)])}});export{Ue as L,oe as c,ze as p};