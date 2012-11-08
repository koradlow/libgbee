var NAVTREE =
[
  [ "LibGBee", "index.html", [
    [ "Main Page", "index.html", null ],
    [ "Related Pages", "pages.html", [
      [ "Porting", "porting.html", null ],
      [ "Utility Functions", "utility_functions.html", null ],
      [ "Build Instructions", "build_instructions.html", null ],
      [ "Building for AT91-SAM7", "building_for_at91_sam7.html", null ],
      [ "Building for Linux on MIPSel", "building_for_linux_mipsel.html", null ],
      [ "Building for Linux on X86", "building_for_linux_x86.html", null ],
      [ "Building for Windows on X86", "building_for_windows_x86.html", null ]
    ] ],
    [ "Data Structures", "annotated.html", [
      [ "gbee", "structgbee.html", null ],
      [ "gbeeAtCommand", "structgbee_at_command.html", null ],
      [ "gbeeAtCommandQueue", "structgbee_at_command_queue.html", null ],
      [ "gbeeAtCommandResponse", "structgbee_at_command_response.html", null ],
      [ "gbeeFrame", "structgbee_frame.html", null ],
      [ "gbeeFrameData", "uniongbee_frame_data.html", null ],
      [ "gbeeFrameHeader", "structgbee_frame_header.html", null ],
      [ "gbeeFrameTrailer", "structgbee_frame_trailer.html", null ],
      [ "gbeeModemStatus", "structgbee_modem_status.html", null ],
      [ "gbeeRemoteAtCommand", "structgbee_remote_at_command.html", null ],
      [ "gbeeRemoteAtCommandResponse", "structgbee_remote_at_command_response.html", null ],
      [ "gbeeRxPacket16", "structgbee_rx_packet16.html", null ],
      [ "gbeeRxPacket64", "structgbee_rx_packet64.html", null ],
      [ "GBeeSockAddr", "struct_g_bee_sock_addr.html", null ],
      [ "gbeeTxRequest16", "structgbee_tx_request16.html", null ],
      [ "gbeeTxRequest64", "structgbee_tx_request64.html", null ],
      [ "gbeeTxStatus", "structgbee_tx_status.html", null ],
      [ "gbeeUsart", "structgbee_usart.html", null ],
      [ "gbeeUsartDevice", "structgbee_usart_device.html", null ]
    ] ],
    [ "Data Structure Index", "classes.html", null ],
    [ "Data Fields", "functions.html", null ],
    [ "File List", "files.html", [
      [ "gbee-heap.c", "gbee-heap_8c.html", null ],
      [ "gbee-heap.h", "gbee-heap_8h.html", null ],
      [ "gbee-linux.c", "gbee-linux_8c.html", null ],
      [ "gbee-linux.h", "gbee-linux_8h.html", null ],
      [ "gbee-port-interface.h", "gbee-port-interface_8h.html", null ],
      [ "at91/sam7/gbee-port.c", "at91_2sam7_2gbee-port_8c.html", null ],
      [ "windows/x86/gbee-port.c", "windows_2x86_2gbee-port_8c.html", null ],
      [ "at91/sam7/gbee-port.h", "at91_2sam7_2gbee-port_8h.html", null ],
      [ "linux/mipsel/gbee-port.h", "linux_2mipsel_2gbee-port_8h.html", null ],
      [ "linux/x86/gbee-port.h", "linux_2x86_2gbee-port_8h.html", null ],
      [ "windows/x86/gbee-port.h", "windows_2x86_2gbee-port_8h.html", null ],
      [ "gbee-tick.c", "gbee-tick_8c.html", null ],
      [ "gbee-tick.h", "gbee-tick_8h.html", null ],
      [ "gbee-usart.c", "gbee-usart_8c.html", null ],
      [ "gbee-usart.h", "gbee-usart_8h.html", null ],
      [ "gbee-util.c", "gbee-util_8c.html", null ],
      [ "gbee-util.h", "gbee-util_8h.html", null ],
      [ "gbee.c", "gbee_8c.html", null ],
      [ "gbee.h", "gbee_8h.html", null ]
    ] ],
    [ "Globals", "globals.html", null ]
  ] ]
];

function createIndent(o,domNode,node,level)
{
  if (node.parentNode && node.parentNode.parentNode)
  {
    createIndent(o,domNode,node.parentNode,level+1);
  }
  var imgNode = document.createElement("img");
  if (level==0 && node.childrenData)
  {
    node.plus_img = imgNode;
    node.expandToggle = document.createElement("a");
    node.expandToggle.href = "javascript:void(0)";
    node.expandToggle.onclick = function() 
    {
      if (node.expanded) 
      {
        $(node.getChildrenUL()).slideUp("fast");
        if (node.isLast)
        {
          node.plus_img.src = node.relpath+"ftv2plastnode.png";
        }
        else
        {
          node.plus_img.src = node.relpath+"ftv2pnode.png";
        }
        node.expanded = false;
      } 
      else 
      {
        expandNode(o, node, false);
      }
    }
    node.expandToggle.appendChild(imgNode);
    domNode.appendChild(node.expandToggle);
  }
  else
  {
    domNode.appendChild(imgNode);
  }
  if (level==0)
  {
    if (node.isLast)
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2plastnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2lastnode.png";
        domNode.appendChild(imgNode);
      }
    }
    else
    {
      if (node.childrenData)
      {
        imgNode.src = node.relpath+"ftv2pnode.png";
      }
      else
      {
        imgNode.src = node.relpath+"ftv2node.png";
        domNode.appendChild(imgNode);
      }
    }
  }
  else
  {
    if (node.isLast)
    {
      imgNode.src = node.relpath+"ftv2blank.png";
    }
    else
    {
      imgNode.src = node.relpath+"ftv2vertline.png";
    }
  }
  imgNode.border = "0";
}

function newNode(o, po, text, link, childrenData, lastNode)
{
  var node = new Object();
  node.children = Array();
  node.childrenData = childrenData;
  node.depth = po.depth + 1;
  node.relpath = po.relpath;
  node.isLast = lastNode;

  node.li = document.createElement("li");
  po.getChildrenUL().appendChild(node.li);
  node.parentNode = po;

  node.itemDiv = document.createElement("div");
  node.itemDiv.className = "item";

  node.labelSpan = document.createElement("span");
  node.labelSpan.className = "label";

  createIndent(o,node.itemDiv,node,0);
  node.itemDiv.appendChild(node.labelSpan);
  node.li.appendChild(node.itemDiv);

  var a = document.createElement("a");
  node.labelSpan.appendChild(a);
  node.label = document.createTextNode(text);
  a.appendChild(node.label);
  if (link) 
  {
    a.href = node.relpath+link;
  } 
  else 
  {
    if (childrenData != null) 
    {
      a.className = "nolink";
      a.href = "javascript:void(0)";
      a.onclick = node.expandToggle.onclick;
      node.expanded = false;
    }
  }

  node.childrenUL = null;
  node.getChildrenUL = function() 
  {
    if (!node.childrenUL) 
    {
      node.childrenUL = document.createElement("ul");
      node.childrenUL.className = "children_ul";
      node.childrenUL.style.display = "none";
      node.li.appendChild(node.childrenUL);
    }
    return node.childrenUL;
  };

  return node;
}

function showRoot()
{
  var headerHeight = $("#top").height();
  var footerHeight = $("#nav-path").height();
  var windowHeight = $(window).height() - headerHeight - footerHeight;
  navtree.scrollTo('#selected',0,{offset:-windowHeight/2});
}

function expandNode(o, node, imm)
{
  if (node.childrenData && !node.expanded) 
  {
    if (!node.childrenVisited) 
    {
      getNode(o, node);
    }
    if (imm)
    {
      $(node.getChildrenUL()).show();
    } 
    else 
    {
      $(node.getChildrenUL()).slideDown("fast",showRoot);
    }
    if (node.isLast)
    {
      node.plus_img.src = node.relpath+"ftv2mlastnode.png";
    }
    else
    {
      node.plus_img.src = node.relpath+"ftv2mnode.png";
    }
    node.expanded = true;
  }
}

function getNode(o, po)
{
  po.childrenVisited = true;
  var l = po.childrenData.length-1;
  for (var i in po.childrenData) 
  {
    var nodeData = po.childrenData[i];
    po.children[i] = newNode(o, po, nodeData[0], nodeData[1], nodeData[2],
        i==l);
  }
}

function findNavTreePage(url, data)
{
  var nodes = data;
  var result = null;
  for (var i in nodes) 
  {
    var d = nodes[i];
    if (d[1] == url) 
    {
      return new Array(i);
    }
    else if (d[2] != null) // array of children
    {
      result = findNavTreePage(url, d[2]);
      if (result != null) 
      {
        return (new Array(i).concat(result));
      }
    }
  }
  return null;
}

function initNavTree(toroot,relpath)
{
  var o = new Object();
  o.toroot = toroot;
  o.node = new Object();
  o.node.li = document.getElementById("nav-tree-contents");
  o.node.childrenData = NAVTREE;
  o.node.children = new Array();
  o.node.childrenUL = document.createElement("ul");
  o.node.getChildrenUL = function() { return o.node.childrenUL; };
  o.node.li.appendChild(o.node.childrenUL);
  o.node.depth = 0;
  o.node.relpath = relpath;

  getNode(o, o.node);

  o.breadcrumbs = findNavTreePage(toroot, NAVTREE);
  if (o.breadcrumbs == null)
  {
    o.breadcrumbs = findNavTreePage("index.html",NAVTREE);
  }
  if (o.breadcrumbs != null && o.breadcrumbs.length>0)
  {
    var p = o.node;
    for (var i in o.breadcrumbs) 
    {
      var j = o.breadcrumbs[i];
      p = p.children[j];
      expandNode(o,p,true);
    }
    p.itemDiv.className = p.itemDiv.className + " selected";
    p.itemDiv.id = "selected";
    $(window).load(showRoot);
  }
}

