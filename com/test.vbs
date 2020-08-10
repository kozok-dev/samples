Set test = CreateObject("COMTest")
a = test.ArrayTest
MsgBox IsArray(a) & " : " & UBound(a), 0, "COMTest"
