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
    bool hasRedirect;
    bool threadFragmented;
};

struct component_information {
    QString componentName;
    QString type;
    QString filename;
    QString version;
    QString remote_version;
    QString src;
    QString target;
};

#endif // STRUCTS_H
