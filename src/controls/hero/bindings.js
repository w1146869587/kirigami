// ⚧

function breakBindings(objectToBreak) {
    objectToBreak.width = objectToBreak.width + 0
    objectToBreak.height = objectToBreak.height + 0
    objectToBreak.implicitWidth = objectToBreak.implicitWidth + 0
    objectToBreak.implicitHeight = objectToBreak.implicitHeight + 0

    objectToBreak.Layout.alignment = (objectToBreak.Layout.alignment)
    objectToBreak.Layout.bottomMargin = objectToBreak.Layout.bottomMargin+0
    objectToBreak.Layout.column = objectToBreak.Layout.column+0
    objectToBreak.Layout.columnSpan = objectToBreak.Layout.columnSpan+0
    objectToBreak.Layout.fillHeight = objectToBreak.Layout.fillHeight+0
    objectToBreak.Layout.fillWidth = objectToBreak.Layout.fillWidth+0
    objectToBreak.Layout.leftMargin = objectToBreak.Layout.leftMargin+0
    objectToBreak.Layout.margins = objectToBreak.Layout.margins+0
    objectToBreak.Layout.maximumHeight = objectToBreak.Layout.maximumHeight+0
    objectToBreak.Layout.maximumWidth = objectToBreak.Layout.maximumWidth+0
    objectToBreak.Layout.minimumHeight = objectToBreak.Layout.minimumHeight+0
    objectToBreak.Layout.minimumWidth = objectToBreak.Layout.minimumWidth+0
    objectToBreak.Layout.preferredHeight = objectToBreak.Layout.preferredHeight+0
    objectToBreak.Layout.preferredWidth = objectToBreak.Layout.preferredWidth+0
    objectToBreak.Layout.rightMargin = objectToBreak.Layout.rightMargin+0
    objectToBreak.Layout.row = objectToBreak.Layout.row+0
    objectToBreak.Layout.rowSpan = objectToBreak.Layout.rowSpan+0
    objectToBreak.Layout.topMargin = objectToBreak.Layout.topMargin+0
}
function restoreBindings(objectToRestore, objectToRestoreFrom) {
    objectToRestore.width = Qt.binding(() => objectToRestoreFrom.width)
    objectToRestore.height = Qt.binding(() => objectToRestoreFrom.height)
    objectToRestore.implicitWidth = Qt.binding(() => objectToRestoreFrom.implicitWidth)
    objectToRestore.implicitHeight = Qt.binding(() => objectToRestoreFrom.implicitHeight)

    objectToRestore.Layout.alignment = Qt.binding(() => objectToRestoreFrom.Layout.alignment)
    objectToRestore.Layout.bottomMargin = Qt.binding(() => objectToRestoreFrom.Layout.bottomMargin)
    objectToRestore.Layout.column = Qt.binding(() => objectToRestoreFrom.Layout.column)
    objectToRestore.Layout.columnSpan = Qt.binding(() => objectToRestoreFrom.Layout.columnSpan)
    objectToRestore.Layout.fillHeight = Qt.binding(() => objectToRestoreFrom.Layout.fillHeight)
    objectToRestore.Layout.fillWidth = Qt.binding(() => objectToRestoreFrom.Layout.fillWidth)
    objectToRestore.Layout.leftMargin = Qt.binding(() => objectToRestoreFrom.Layout.leftMargin)
    objectToRestore.Layout.margins = Qt.binding(() => objectToRestoreFrom.Layout.margins)
    objectToRestore.Layout.maximumHeight = Qt.binding(() => objectToRestoreFrom.Layout.maximumHeight)
    objectToRestore.Layout.maximumWidth = Qt.binding(() => objectToRestoreFrom.Layout.maximumWidth)
    objectToRestore.Layout.minimumHeight = Qt.binding(() => objectToRestoreFrom.Layout.minimumHeight)
    objectToRestore.Layout.minimumWidth = Qt.binding(() => objectToRestoreFrom.Layout.minimumWidth)
    objectToRestore.Layout.preferredHeight = Qt.binding(() => objectToRestoreFrom.Layout.preferredHeight)
    objectToRestore.Layout.preferredWidth = Qt.binding(() => objectToRestoreFrom.Layout.preferredWidth)
    objectToRestore.Layout.rightMargin = Qt.binding(() => objectToRestoreFrom.Layout.rightMargin)
    objectToRestore.Layout.row = Qt.binding(() => objectToRestoreFrom.Layout.row)
    objectToRestore.Layout.rowSpan = Qt.binding(() => objectToRestoreFrom.Layout.rowSpan)
    objectToRestore.Layout.topMargin = Qt.binding(() => objectToRestoreFrom.Layout.topMargin)
}