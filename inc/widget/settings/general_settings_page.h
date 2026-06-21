#ifndef GENERAL_SETTINGS_PAGE_H
#define GENERAL_SETTINGS_PAGE_H

#include "main.h"

class QListWidgetItem;

namespace Ui {
class GeneralSettingsPage;
}

class GeneralSettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralSettingsPage(QWidget *parent = nullptr);
    ~GeneralSettingsPage();

    void loadSettings();
    void saveSettings();

private slots:
    void onFontCategoryChanged(int index);
    void onFontFamilyChanged(int index);
    void onFontSizeChanged(int index);
    void onPreviewTextChanged();

private:
    void populateFontCategories();
    void populateFontFamilies();
    void populateFontSizes();
    void updateFontPreview();
    void populatePanelList();

    Ui::GeneralSettingsPage *ui;
};

#endif // GENERAL_SETTINGS_PAGE_H
