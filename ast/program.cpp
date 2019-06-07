#include <utility>
#include "program.h"

using namespace std;
using namespace gw_logic;

ast::Line::Line(int lineNumber, std::vector<std::unique_ptr<ast::Expression>> statementList, std::string comment):
        lineNumber(lineNumber),
        statementList(std::move(statementList)),
        comment(std::move(comment)) { }

void ast::Line::provideInfo(ast::ProgramInfo& programInfo) const {
    for (auto& statement : statementList) {
        statement->provideInfo(programInfo);
    }
    programInfo.coreFiles.insert(gw_logic::core_core);
}

void ast::Line::print(ostream& stream) const {
    stream << "line(" << lineNumber << ");";

    if (comment[0] != '\0') {
        stream << " //" << comment;
    }

    stream << endl;

    for (auto& statement : statementList) {
        stream << "    ";
        statement->print(stream);
        stream << ';' << endl;
    }
}

void ast::printCoreFile(std::ostream& stream,
                        const gw_logic::CoreFile* coreFile,
                        std::set<const gw_logic::CoreFile*>& printedCoreFiles) {
    bool inserted = printedCoreFiles.insert(coreFile).second;
    if (!inserted) {
        return;
    }

    for (auto dependency : coreFile->coreDependencyList) {
        printCoreFile(stream, dependency, printedCoreFiles);
    }

    stream << coreFile->code << endl;
}

void ast::printLogicFile(std::ostream& stream,
                         const gw_logic::LogicFile* logicFile,
                         std::set<const gw_logic::CoreFile*>& printedCoreFiles,
                         std::set<const gw_logic::LogicFile*>& printedLogicFiles) {
    bool inserted = printedLogicFiles.insert(logicFile).second;
    if (!inserted) {
        return;
    }

    for (auto dependency : logicFile->coreDependencyList) {
        printCoreFile(stream, dependency, printedCoreFiles);
    }
    for (auto dependency : logicFile->logicDependencyList) {
        printLogicFile(stream, dependency, printedCoreFiles, printedLogicFiles);
    }

    stream << logicFile->code << endl;
}

void ast::printProgram(std::ostream& stream, std::vector<ast::Line> lines) {
    std::sort(lines.begin(), lines.end(), [](ast::Line& a, ast::Line& b) {
        return a.lineNumber < b.lineNumber;
    });

    ProgramInfo programInfo;

    for (auto& line : lines) {
        line.provideInfo(programInfo);
    }

    std::set<const gw_logic::CoreFile*> printedCoreFiles;
    std::set<const gw_logic::LogicFile*> printedLogicFiles;

    for (auto coreFile : programInfo.coreFiles) {
        printCoreFile(stream, coreFile, printedCoreFiles);
    }
    for (auto logicFile : programInfo.logicFiles) {
        printLogicFile(stream, logicFile, printedCoreFiles, printedLogicFiles);
    }

    stream << endl;

    for (auto& definition : programInfo.variableDefinitions) {
        stream << definition << ';' << endl;
    }

    stream << endl;

    stream << "int main() {" << endl;

    for (auto& line : lines) {
        line.print(stream);
    }

    stream << "}" << endl;
}
