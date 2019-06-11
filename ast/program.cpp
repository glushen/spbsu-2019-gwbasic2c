#include <utility>
#include "program.h"

ast::Line::Line(int lineNumber, std::vector<std::unique_ptr<ast::Expression>> statementList, std::string comment):
        lineNumber(lineNumber),
        statementList(std::move(statementList)),
        comment(std::move(comment)) { }

void ast::Line::provideInfo(ast::ProgramInfo& programInfo) const {
    for (auto& statement : statementList) {
        statement->provideInfo(programInfo);
    }
    programInfo.coreFiles.insert(gw::core::core);
}

void ast::Line::print(std::ostream& stream) const {
    stream << "L" << lineNumber << ": line(" << lineNumber << ");";

    if (comment[0] != '\0') {
        stream << " //" << comment;
    }

    stream << std::endl;

    for (auto& statement : statementList) {
        stream << "    ";
        statement->print(stream);
        stream << ';' << std::endl;
    }
}

void ast::printCoreFile(std::ostream& stream,
                        const gw::core::File* coreFile,
                        std::set<const gw::core::File*>& printedCoreFiles) {
    bool inserted = printedCoreFiles.insert(coreFile).second;
    if (!inserted) {
        return;
    }

    for (auto dependency : coreFile->coreDependencies) {
        printCoreFile(stream, dependency, printedCoreFiles);
    }

    stream << coreFile->code << std::endl;
}

void ast::printLogicFile(std::ostream& stream,
                         const gw::logic::File* logicFile,
                         std::set<const gw::core::File*>& printedCoreFiles,
                         std::set<const gw::logic::File*>& printedLogicFiles) {
    bool inserted = printedLogicFiles.insert(logicFile).second;
    if (!inserted) {
        return;
    }

    for (auto dependency : logicFile->coreDependencies) {
        printCoreFile(stream, dependency, printedCoreFiles);
    }
    for (auto dependency : logicFile->logicDependencies) {
        printLogicFile(stream, dependency, printedCoreFiles, printedLogicFiles);
    }

    stream << logicFile->code << std::endl;
}

void ast::printProgram(std::ostream& stream, std::vector<ast::Line> lines) {
    std::stable_sort(lines.begin(), lines.end(), [](const ast::Line& a, const ast::Line& b) {
        return a.lineNumber < b.lineNumber;
    });
    std::reverse(lines.begin(), lines.end());
    lines.erase(std::unique(lines.begin(), lines.end(), [](ast::Line& a, ast::Line& b) {
        return a.lineNumber == b.lineNumber;
    }), lines.end());
    std::reverse(lines.begin(), lines.end());

    std::set<int> existentLineNumbers;
    for (auto& line : lines) {
        existentLineNumbers.insert(line.lineNumber);
    }

    ProgramInfo programInfo;

    for (auto& line : lines) {
        line.provideInfo(programInfo);
        if (!std::includes(existentLineNumbers.begin(), existentLineNumbers.end(),
                           programInfo.requiredLineNumbers.begin(), programInfo.requiredLineNumbers.end())) {
            throw std::invalid_argument("Used line numbers are not found");
        }
        programInfo.requiredLineNumbers.clear();
    }

    if (programInfo.openedWhileLoopsCount > 0) {
        throw std::invalid_argument("No WEND for " + std::to_string(programInfo.openedWhileLoopsCount) + " WHILE(s)");
    }

    if (!programInfo.forLoopsVariables.empty()) {
        throw std::invalid_argument("No NEXT for " + std::to_string(programInfo.forLoopsVariables.size()) + " FOR(s)");
    }

    std::set<const gw::core::File*> printedCoreFiles;
    std::set<const gw::logic::File*> printedLogicFiles;

    for (auto coreFile : programInfo.coreFiles) {
        printCoreFile(stream, coreFile, printedCoreFiles);
    }
    for (auto logicFile : programInfo.logicFiles) {
        printLogicFile(stream, logicFile, printedCoreFiles, printedLogicFiles);
    }

    stream << std::endl;

    for (auto& definition : programInfo.variableDefinitions) {
        stream << definition << ';' << std::endl;
    }

    stream << std::endl;

    stream << "int main() {" << std::endl;

    for (auto& line : lines) {
        line.print(stream);
    }

    stream << "}" << std::endl;
}
