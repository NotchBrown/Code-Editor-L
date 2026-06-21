#ifndef EDITOR_SETTINGS_PAGE_H
#define EDITOR_SETTINGS_PAGE_H

#include "main.h"

namespace Ui {
class EditorSettingsPage;
}

class EditorSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit EditorSettingsPage(QWidget *parent = nullptr);
    ~EditorSettingsPage();

    void loadSettings();
    void saveSettings();

private slots:
    void onLanguageChanged(int index);
    void onFontCategoryChanged(int index);
    void onFontFamilyChanged(int index);
    void onFontSizeChanged(int index);
    void onSettingChanged();

private:
    void populateLanguageList();
    void populateFontCategories();
    void populateFontFamilies();
    void populateFontSizes();
    void loadCurrentLanguageSettings();
    void updatePreview();

    Ui::EditorSettingsPage *ui;
    QString m_currentLang; // 当前正在编辑的语言
    bool m_loading = false;
};

#endif // EDITOR_SETTINGS_PAGE_H
