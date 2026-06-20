#ifndef SEGMENT_H
#define SEGMENT_H

#include "main.h"
#include "component/language_component.h"

class CodeEditor;

namespace Ui {
class Segment;
}

class Segment : public QWidget
{
    Q_OBJECT

public:
    explicit Segment(QWidget *parent = nullptr);
    ~Segment();

    // Update segment info based on cursor position
    void updateAtPosition(CodeEditor *editor, int line, int column);

private:
    void clearInfo();
    void addInfoRow(const QString &label, const QString &value);

    Ui::Segment *ui;
};

#endif // SEGMENT_H