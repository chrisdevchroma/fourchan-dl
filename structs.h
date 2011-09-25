#ifndef STRUCTS_H
#define STRUCTS_H

struct _IMAGE
{
    QString largeURI;
    QString thumbURI;
    QString originalFilename;
    QString savedAs;
    bool downloaded;
    bool requested;
};

struct ParsingStatus
{
    bool hasImages;
    bool hasTitle;
    bool isFrontpage;
    bool hasErrors;
};

#endif // STRUCTS_H
