Set objArgs = WScript.Arguments
If objArgs.Count < 1 Then
    WScript.Echo "Usage: cscript ico2hpp.vbs input.ico [output_folder]"
    WScript.Quit 1
End If

inputFile = objArgs(0)
inputFile = Trim(inputFile)

If Not CreateObject("Scripting.FileSystemObject").FileExists(inputFile) Then
    WScript.Echo "File not found: " & inputFile
    WScript.Quit 1
End If

If objArgs.Count > 1 Then
    outputFolder = objArgs(1)
Else
    outputFolder = CreateObject("Scripting.FileSystemObject").GetParentFolderName(WScript.ScriptFullName)
End If

outputFile = outputFolder & "\StormTrackIconData.hpp"
arrayName = "StormTrackIconData"
arraySizeName = "StormTrackIconDataSize"

Set stream = CreateObject("ADODB.Stream")
stream.Type = 1
stream.Open()
stream.LoadFromFile inputFile
size = stream.Size
Dim bytes()
ReDim bytes(size - 1)
For i = 0 To size - 1
    stream.Position = i
    bytes(i) = AscB(stream.Read(1))
Next
stream.Close()

output = "#pragma once" & vbCrLf & vbCrLf
output = output & "#include <cstddef>" & vbCrLf & vbCrLf
output = output & "namespace icon {" & vbCrLf & vbCrLf
output = output & "    static const unsigned char " & arrayName & "[] = {" & vbCrLf
output = output & "        "

hexLine = ""
For i = 0 To UBound(bytes)
    b = bytes(i)
    hexValue = Hex(b)
    If Len(hexValue) = 1 Then hexValue = "0" & hexValue
    hexLine = hexLine & "0x" & hexValue
    If i < UBound(bytes) Then
        hexLine = hexLine & ", "
        If (i + 1) Mod 16 = 0 Then
            output = output & hexLine & vbCrLf & "        "
            hexLine = ""
        End If
    End If
Next

If hexLine <> "" Then
    output = output & hexLine
End If

output = output & vbCrLf & "    };" & vbCrLf & vbCrLf
output = output & "    static const size_t " & arraySizeName & " = sizeof(" & arrayName & ");" & vbCrLf & vbCrLf
output = output & "} // namespace icon" & vbCrLf

Set fso = CreateObject("Scripting.FileSystemObject")

If Not fso.FolderExists(outputFolder) Then
    fso.CreateFolder(outputFolder)
End If

Set outFile = fso.CreateTextFile(outputFile, True)
outFile.Write output
outFile.Close()