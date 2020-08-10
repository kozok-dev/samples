VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form Form1 
   Caption         =   "Oekaki"
   ClientHeight    =   5895
   ClientLeft      =   1575
   ClientTop       =   2160
   ClientWidth     =   7455
   LinkTopic       =   "Form1"
   ScaleHeight     =   5895
   ScaleWidth      =   7455
   Begin VB.Timer Timer1 
      Enabled         =   0   'False
      Interval        =   20
      Left            =   2760
      Top             =   120
   End
   Begin VB.PictureBox Field 
      AutoRedraw      =   -1  'True
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00000000&
      Height          =   4935
      Left            =   0
      ScaleHeight     =   325
      ScaleMode       =   3  'ﾋﾟｸｾﾙ
      ScaleWidth      =   493
      TabIndex        =   13
      Top             =   960
      Width           =   7455
   End
   Begin MSComDlg.CommonDialog ComD 
      Left            =   2160
      Top             =   120
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.Frame Frame1 
      Caption         =   "Color"
      Height          =   855
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   2055
      Begin VB.PictureBox FCP 
         BackColor       =   &H00000000&
         Height          =   375
         Left            =   1440
         ScaleHeight     =   315
         ScaleWidth      =   315
         TabIndex        =   12
         Top             =   240
         Width           =   375
      End
      Begin VB.PictureBox BCP 
         BackColor       =   &H00FFFFFF&
         Height          =   375
         Left            =   1560
         ScaleHeight     =   315
         ScaleWidth      =   315
         TabIndex        =   11
         Top             =   360
         Width           =   375
      End
      Begin VB.PictureBox CP 
         BackColor       =   &H00FF8080&
         Height          =   255
         Index           =   9
         Left            =   1080
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   10
         Top             =   480
         Width           =   255
      End
      Begin VB.PictureBox CP 
         BackColor       =   &H0000FFFF&
         Height          =   255
         Index           =   8
         Left            =   840
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   9
         Top             =   480
         Width           =   255
      End
      Begin VB.PictureBox CP 
         BackColor       =   &H008080FF&
         Height          =   255
         Index           =   7
         Left            =   600
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   8
         Top             =   480
         Width           =   255
      End
      Begin VB.PictureBox CP 
         BackColor       =   &H00808080&
         Height          =   255
         Index           =   6
         Left            =   360
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   7
         Top             =   480
         Width           =   255
      End
      Begin VB.PictureBox CP 
         BackColor       =   &H00C0C0C0&
         Height          =   255
         Index           =   5
         Left            =   120
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   6
         Top             =   480
         Width           =   255
      End
      Begin VB.PictureBox CP 
         BackColor       =   &H00FF0000&
         Height          =   255
         Index           =   4
         Left            =   1080
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   5
         Top             =   240
         Width           =   255
      End
      Begin VB.PictureBox CP 
         BackColor       =   &H0000FF00&
         Height          =   255
         Index           =   3
         Left            =   840
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   4
         Top             =   240
         Width           =   255
      End
      Begin VB.PictureBox CP 
         BackColor       =   &H000000FF&
         Height          =   255
         Index           =   2
         Left            =   600
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   3
         Top             =   240
         Width           =   255
      End
      Begin VB.PictureBox CP 
         BackColor       =   &H00FFFFFF&
         Height          =   255
         Index           =   1
         Left            =   360
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   2
         Top             =   240
         Width           =   255
      End
      Begin VB.PictureBox CP 
         BackColor       =   &H00000000&
         Height          =   255
         Index           =   0
         Left            =   120
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   1
         Top             =   240
         Width           =   255
      End
   End
   Begin VB.Menu m1 
      Caption         =   "メイン(&M)"
      Begin VB.Menu m11 
         Caption         =   "すべて消去"
      End
      Begin VB.Menu mn1 
         Caption         =   "-"
      End
      Begin VB.Menu m12 
         Caption         =   "ピクチャの読み込み"
      End
      Begin VB.Menu m13 
         Caption         =   "ピクチャの書き込み"
      End
      Begin VB.Menu mn2 
         Caption         =   "-"
      End
      Begin VB.Menu m14 
         Caption         =   "終了"
      End
   End
   Begin VB.Menu m2 
      Caption         =   "線幅(&L)"
      Begin VB.Menu m21 
         Caption         =   "1"
         Checked         =   -1  'True
         Index           =   0
      End
      Begin VB.Menu m21 
         Caption         =   "2"
         Index           =   1
      End
      Begin VB.Menu m21 
         Caption         =   "4"
         Index           =   2
      End
      Begin VB.Menu m21 
         Caption         =   "8"
         Index           =   3
      End
      Begin VB.Menu m21 
         Caption         =   "16"
         Index           =   4
      End
      Begin VB.Menu m21 
         Caption         =   "32"
         Index           =   5
      End
   End
   Begin VB.Menu m3 
      Caption         =   "種類(&S)"
      Begin VB.Menu m31 
         Caption         =   "自由"
         Checked         =   -1  'True
         Index           =   0
      End
      Begin VB.Menu m31 
         Caption         =   "線"
         Index           =   1
      End
      Begin VB.Menu m31 
         Caption         =   "四角形"
         Index           =   2
      End
      Begin VB.Menu m31 
         Caption         =   "楕円"
         Index           =   3
      End
      Begin VB.Menu m31 
         Caption         =   "塗りつぶし"
         Index           =   4
      End
   End
   Begin VB.Menu m4 
      Caption         =   "スクリプト(&F)"
      Begin VB.Menu m41 
         Caption         =   "実行"
      End
      Begin VB.Menu m42 
         Caption         =   "録画"
      End
      Begin VB.Menu m43 
         Caption         =   "停止"
         Enabled         =   0   'False
      End
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Declare Function Rectangle Lib "gdi32" (ByVal hdc As Long, ByVal X1 As Long, ByVal Y1 As Long, ByVal X2 As Long, ByVal Y2 As Long) As Long
Private Declare Function Ellipse Lib "gdi32" (ByVal hdc As Long, ByVal X1 As Long, ByVal Y1 As Long, ByVal X2 As Long, ByVal Y2 As Long) As Long
Private Declare Function ExtFloodFill Lib "gdi32" (ByVal hdc As Long, ByVal x As Long, ByVal y As Long, ByVal crColor As Long, ByVal wFillType As Long) As Long

Option Explicit
Dim dflag As Boolean, sflag As Boolean, dstyle As Long
Dim sx As Long, sy As Long

Private Sub BCP_DblClick()
    On Error Resume Next
    ComD.ShowColor
    BCP.BackColor = ComD.Color
End Sub

Private Sub CP_MouseUp(Index As Integer, Button As Integer, Shift As Integer, x As Single, y As Single)
    If Button = vbLeftButton Then
        FCP.BackColor = CP(Index).BackColor
    Else
        BCP.BackColor = CP(Index).BackColor
    End If
End Sub

Private Sub FCP_DblClick()
    On Error Resume Next
    ComD.ShowColor
    FCP.BackColor = ComD.Color
End Sub

Private Sub Field_MouseDown(Button As Integer, Shift As Integer, x As Single, y As Single)
    If Timer1.Enabled = True Then Exit Sub
    dflag = True

    If Button = vbLeftButton Then
        Field.ForeColor = FCP.BackColor
    Else
        Field.ForeColor = BCP.BackColor
    End If

    Select Case dstyle
        Case 0
            Field.PSet (x, y)

            If sflag = True Then
                Print #2, "C" & Field.ForeColor
                Print #2, "P" & x & "," & y
            End If
        Case 1
            sx = x
            sy = y
        Case 2
            sx = x
            sy = y
        Case 3
            sx = x
            sy = y
        Case 4
            Field.FillStyle = vbFSSolid
            Field.FillColor = Field.ForeColor
            ExtFloodFill Field.hdc, x, y, Field.Point(x, y), 1

            If sflag = True Then
                Print #2, "C" & Field.ForeColor
                Print #2, "F" & x & "," & y
            End If
    End Select
End Sub

Private Sub Field_MouseMove(Button As Integer, Shift As Integer, x As Single, y As Single)
    If dflag = False Or Timer1.Enabled = True Then Exit Sub

    If dstyle = 0 Then Field.Line -(x, y)

    If sflag = True And dstyle = 0 Then Print #2, "P" & x & "," & y
End Sub

Private Sub Field_MouseUp(Button As Integer, Shift As Integer, x As Single, y As Single)
    If Timer1.Enabled = True Then Exit Sub
    dflag = False

    Select Case dstyle
        Case 1
            Field.Line (sx, sy)-(x, y)

            If sflag = True Then
                Print #2, "C" & Field.ForeColor
                Print #2, "L" & sx & "," & sy & "," & x & "," & y
            End If
        Case 2
            Field.FillStyle = vbFSTransparent
            Rectangle Field.hdc, sx, sy, x, y
            Field.PSet (-32, -32)

            If sflag = True Then
                Print #2, "C" & Field.ForeColor
                Print #2, "R" & sx & "," & sy & "," & x & "," & y
            End If
        Case 3
            Field.FillStyle = vbFSTransparent
            Ellipse Field.hdc, sx, sy, x, y
            Field.PSet (-32, -32)

            If sflag = True Then
                Print #2, "C" & Field.ForeColor
                Print #2, "E" & sx & "," & sy & "," & x & "," & y
            End If
    End Select
End Sub

Private Sub Form_Load()
    ComD.CancelError = True
    dflag = False
    sflag = False
    dstyle = 0
End Sub

Private Sub Form_Resize()
    On Error Resume Next
    Field.Width = Form1.Width - Field.Left - 115
    Field.Height = Form1.Height - Field.Top - 685
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Close #1
    Close #2
End Sub

Private Sub m11_Click()
    Field.Cls
    Field.Picture = LoadPicture("")
End Sub

Private Sub m12_Click()
    On Error GoTo openerr
    ComD.Filter = "対応ピクチャ|*.bmp;*.jpg;*.jpeg;*.gif|すべてのファイル(*.*)|*.*|"
    ComD.ShowOpen
    ComD.InitDir = ""
    Field.Picture = LoadPicture(ComD.FileName)

openerr:
End Sub

Private Sub m13_Click()
    On Error GoTo wrierr
    ComD.Filter = "すべてのファイル(*.*)|*.*|対応ピクチャ|*.bmp;*.jpg;*.jpeg;*.gif|"
    ComD.DefaultExt = "bmp"
    ComD.ShowSave
    ComD.InitDir = ""
    SavePicture Field.Image, ComD.FileName

wrierr:
End Sub

Private Sub m14_Click()
    Unload Me
End Sub

Private Sub m21_Click(Index As Integer)
    Dim i As Long
    For i = 0 To m21.Count - 1
        m21(i).Checked = False
    Next
    m21(Index).Checked = True

    Field.DrawWidth = m21(Index).Caption

    If sflag = True Then Print #2, "W" & Field.DrawWidth
End Sub

Private Sub m31_Click(Index As Integer)
    Dim i As Long
    For i = 0 To m31.Count - 1
        m31(i).Checked = False
    Next
    m31(Index).Checked = True

    dstyle = Index
End Sub

Private Sub m41_Click()
    On Error GoTo openerr
    ComD.Filter = "OEKAKI Script(*.kos)|*.kos|すべてのファイル(*.*)|*.*|"
    ComD.ShowOpen
    ComD.InitDir = ""

    Dim kos As String
    Open ComD.FileName For Input As #1
    Line Input #1, kos$
    If kos$ <> "[OEKAKI Script] by Kozo" Then Exit Sub

    Field.Cls
    Field.Picture = LoadPicture("")

    m41.Enabled = False
    m42.Enabled = False
    m43.Enabled = True

    Timer1.Enabled = True

openerr:
End Sub

Private Sub m42_Click()
    On Error GoTo wrierr
    ComD.Filter = "OEKAKI Script(*.kos)|*.kos|すべてのファイル(*.*)|*.*|"
    ComD.DefaultExt = "kos"
    ComD.ShowSave
    ComD.InitDir = ""

    Open ComD.FileName For Output As #2
    Print #2, "[OEKAKI Script] by Kozo" & vbCrLf
    Print #2, "W" & Field.DrawWidth

    m41.Enabled = False
    m42.Enabled = False
    m43.Enabled = True

    sflag = True

wrierr:
End Sub

Private Sub m43_Click()
    If Timer1.Enabled = True Then
        Timer1.Enabled = False
    Else
        Close #2
        sflag = False
    End If

    m41.Enabled = True
    m42.Enabled = True
    m43.Enabled = False
End Sub

Private Sub Timer1_Timer()
    Dim kos As String
    Dim osx1 As Long, osy1 As Long, osx2 As Long, osy2 As Long
    Static stopper As Boolean

    If EOF(1) = False Then
        Line Input #1, kos$
        Select Case Left$(kos$, 1)
            Case "C"
                Field.ForeColor = Int(Mid$(kos$, 2, Len(kos$) - 1))
                stopper = True
            Case "P"
                osx1 = Int(Mid$(kos$, 2, InStr(2, kos$, ",") - 2))
                osy1 = Int(Mid$(kos$, InStr(2, kos$, ",") + 1, Len(kos$) - InStr(2, kos$, ",")))
                If stopper = True Then
                    Field.PSet (osx1, osy1)
                    stopper = False
                Else
                    Field.Line -(osx1, osy1)
                End If
            Case "W"
                Field.DrawWidth = Int(Mid$(kos$, 2, Len(kos$) - 1))
            Case "L"
                osx1 = Int(Mid$(kos$, 2, InStr(2, kos$, ",") - 2))
                osy1 = Int(Mid$(kos$, InStr(2, kos$, ",") + 1, InStr(InStr(2, kos$, ",") + 1, kos$, ",") - InStr(2, kos$, ",")))
                osx2 = Int(Mid$(kos$, InStr(InStr(2, kos$, ",") + 1, kos$, ",") + 1, InStr(InStr(InStr(2, kos$, ",") + 1, kos$, ",") + 1, kos$, ",") - InStr(InStr(2, kos$, ",") + 1, kos$, ",")))
                osy2 = Int(Mid$(kos$, InStr(InStr(InStr(2, kos$, ",") + 1, kos$, ",") + 1, kos$, ",") + 1, Len(kos$) - InStr(InStr(InStr(2, kos$, ",") + 1, kos$, ",") + 1, kos$, ",")))

                Field.FillStyle = vbFSTransparent
                Field.Line (osx1, osy1)-(osx2, osy2)
                Field.PSet (-32, -32)
            Case "R"
                osx1 = Int(Mid$(kos$, 2, InStr(2, kos$, ",") - 2))
                osy1 = Int(Mid$(kos$, InStr(2, kos$, ",") + 1, InStr(InStr(2, kos$, ",") + 1, kos$, ",") - InStr(2, kos$, ",")))
                osx2 = Int(Mid$(kos$, InStr(InStr(2, kos$, ",") + 1, kos$, ",") + 1, InStr(InStr(InStr(2, kos$, ",") + 1, kos$, ",") + 1, kos$, ",") - InStr(InStr(2, kos$, ",") + 1, kos$, ",")))
                osy2 = Int(Mid$(kos$, InStr(InStr(InStr(2, kos$, ",") + 1, kos$, ",") + 1, kos$, ",") + 1, Len(kos$) - InStr(InStr(InStr(2, kos$, ",") + 1, kos$, ",") + 1, kos$, ",")))

                Field.FillStyle = vbFSTransparent
                Rectangle Field.hdc, osx1, osy1, osx2, osy2
                Field.PSet (-32, -32)
            Case "E"
                osx1 = Int(Mid$(kos$, 2, InStr(2, kos$, ",") - 2))
                osy1 = Int(Mid$(kos$, InStr(2, kos$, ",") + 1, InStr(InStr(2, kos$, ",") + 1, kos$, ",") - InStr(2, kos$, ",")))
                osx2 = Int(Mid$(kos$, InStr(InStr(2, kos$, ",") + 1, kos$, ",") + 1, InStr(InStr(InStr(2, kos$, ",") + 1, kos$, ",") + 1, kos$, ",") - InStr(InStr(2, kos$, ",") + 1, kos$, ",")))
                osy2 = Int(Mid$(kos$, InStr(InStr(InStr(2, kos$, ",") + 1, kos$, ",") + 1, kos$, ",") + 1, Len(kos$) - InStr(InStr(InStr(2, kos$, ",") + 1, kos$, ",") + 1, kos$, ",")))

                Field.FillStyle = vbFSTransparent
                Ellipse Field.hdc, osx1, osy1, osx2, osy2
                Field.PSet (-32, -32)
            Case "F"
                osx1 = Int(Mid$(kos$, 2, InStr(2, kos$, ",") - 2))
                osy1 = Int(Mid$(kos$, InStr(2, kos$, ",") + 1, Len(kos$) - InStr(2, kos$, ",")))

                Field.FillStyle = vbFSSolid
                Field.FillColor = Field.ForeColor
                ExtFloodFill Field.hdc, osx1, osy1, Field.Point(osx1, osy1), 1
        End Select
    Else
        Timer1.Enabled = False
        Close #1

        m41.Enabled = True
        m42.Enabled = True
        m43.Enabled = False

        Dim i As Long
        For i = 0 To m21.Count - 1
        Next i
    End If
End Sub
