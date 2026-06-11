# Properties Window Design Documentation

## 1. Overview

The Properties window provides users with detailed information about the currently opened file, allowing them to view and modify certain file attributes. This document outlines the design and implementation of the Properties window for CodeEditorLite.

## 2. Architecture

### 2.1 Component Structure

```
Properties Window
├── QTableWidget (propertiesTable) - Main table for displaying properties
├── QPushButton (applyButton) - Apply changes
├── QPushButton (refreshButton) - Refresh properties
└── QPushButton (closeButton) - Close window
```

### 2.2 Class Hierarchy

```
QWidget
└── Properties (widget/properties/properties.h)
    ├── setFilePath(const QString&) - Set the file path to display properties for
    ├── refreshProperties() - Refresh the displayed properties
    └── loadProperties() - Load properties from QFileInfo
```

### 2.3 Data Flow

1. User opens Properties window from File menu
2. MainWindow retrieves current file path from active CodeEditor
3. Properties window loads file information using QFileInfo
4. Properties are displayed in a table with editable status
5. User edits editable properties
6. User clicks Apply to save changes
7. Changes are validated and applied to the file system

## 3. UI Components

### 3.1 Properties Table

| Column | Name | Description | Editable |
|--------|------|-------------|----------|
| 0 | Property | Name of the property | No |
| 1 | Value | Value of the property | Depends on property |
| 2 | Editable | Indicates if property can be modified | No |

### 3.2 Buttons

| Button | Action |
|--------|--------|
| Apply | Save changes to file system |
| Refresh | Reload properties from file system |
| Close | Close the window |

## 4. Property Classification

### 4.1 Read-Only Properties (Cannot be modified)

| Property | Description | Source |
|----------|-------------|--------|
| File Name | Display name of the file | QFileInfo::fileName() |
| File Path | Full absolute path | QFileInfo::absoluteFilePath() |
| File Size | Size in bytes (formatted) | QFileInfo::size() |
| File Type | MIME type or extension type | QFileInfo::suffix() |
| Created | Creation date and time | QFileInfo::created() |
| Last Modified | Last modification date and time | QFileInfo::lastModified() |
| Last Accessed | Last access date and time | QFileInfo::lastRead() |
| Owner | File owner (Unix) | QFileInfo::owner() |
| Group | File group (Unix) | QFileInfo::group() |
| Permissions | File permission flags | QFileInfo::permissions() |
| Is Read Only | Read-only status | QFileInfo::isReadOnly() |
| Is Hidden | Hidden file status | QFileInfo::isHidden() |
| Is Directory | Directory status | QFileInfo::isDir() |
| Drive | Drive letter (Windows) | QFileInfo::drive() |

### 4.2 Editable Properties

| Property | Description | Source |
|----------|-------------|--------|
| New File Name | New name for the file | User input |

## 5. Implementation Details

### 5.1 File Renaming Logic

```cpp
// Validate new file name
// Check for invalid characters
// Check if file already exists
// Perform rename operation
// Update editor reference if successful
```

### 5.2 Cross-Platform Considerations

- **Windows**: Handle drive letters, backslashes in paths, NTFS permissions
- **Unix/Linux**: Handle forward slashes, file permissions, owner/group
- **macOS**: Handle bundle structures, extended attributes

### 5.3 Error Handling

- File not found
- Permission denied
- Invalid file name
- File already exists
- Network drive issues

## 6. Slot Functions

### 6.1 onCellChanged(int row, int column)

- Triggered when a cell value is changed
- Validates input for editable properties
- Highlights modified cells

### 6.2 onApplyButtonClicked()

- Collects modified values from table
- Validates all changes
- Applies changes to file system
- Shows success/error message

### 6.3 onRefreshButtonClicked()

- Reloads properties from QFileInfo
- Updates table display
- Clears any unsaved changes

## 7. Dependencies

| Dependency | Purpose |
|------------|---------|
| QFileInfo | Retrieve file metadata |
| QTableWidget | Display properties table |
| QMessageBox | Show error/success messages |
| CodeEditor | Get current file path |

## 8. Future Enhancements

- Batch property editing
- Property export/import
- Advanced file metadata (EXIF, MP3 tags, etc.)
- Custom property plugins
- Property comparison between files

## 9. File Structure

```
widget/properties/
├── properties.h      # Header file with class declaration
├── properties.cpp    # Implementation file
└── properties.ui     # Qt Designer UI file

function/file/
└── on_file_properties.cpp  # Slot function for opening Properties window
```
