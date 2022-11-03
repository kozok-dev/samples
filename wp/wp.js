var pathlist = new Array(
	"C:\\Windows\\Web\\Wallpaper"
);

var fso = new ActiveXObject("Scripting.FileSystemObject");
var stm = new ActiveXObject("ADODB.Stream");
stm.Charset = "UTF-8";
stm.Open();

for (var i in pathlist) {
	writeImgPath(stm, pathlist[i]);
}

stm.SaveToFile(fso.getParentFolderName(WScript.ScriptFullName) + "\\wp.txt", 2);
stm.Close();
WScript.Echo("Š®—¹");

function writeImgPath(stm, path) {
	var folder = fso.GetFolder(path);

	var em = new Enumerator(folder.Files);
	for (em.moveFirst(); !em.atEnd(); em.moveNext()) {
		if (em.item().Path.match(/\.(gif|jpe?g|png)$/i)) {
			stm.WriteText("file://" + em.item().Path.replace(/\\/g, "/"), 1);
		}
	}

	em = new Enumerator(folder.SubFolders);
	for (em.moveFirst(); !em.atEnd(); em.moveNext()) {
		writeImgPath(stm, em.item().Path);
	}
}
