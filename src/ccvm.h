#include <string>
#include <vector>
#include <sstream>

class TagTable {
    static int next_slot;

   public:
    static void init();
    static std::string* create_tag_G();
};


class VMAbstract{
public:
    virtual std::string output()=0;
};

class VMString:VMAbstract{
    std::string str;
    VMString(std::string str):str(str){}
    std::string output() override{
        return str;
    }
};
class VMBlock:VMAbstract{
    VMAbstract *prefix;
    std::vector<VMAbstract*> children;

    VMBlock(VMAbstract *prefix,std::vector<VMAbstract*> children):prefix(prefix),children(children){}

    std::string output() override{
        std::stringstream ss;
        ss<<prefix->output()+"{"<<std::endl;
        for(auto child:children){
            ss<<child->output()<<std::endl;
        }
        ss<<"}"<<std::endl;
    }
};


class VMBranch:VMAbstract{
    Value *cond;
    VMAbstract *ok,*no;
    VMBranch(VMAbstract *cond,VMAbstract *ok,VMAbstract *no):cond(cond),ok(ok),no(no){}
    std::string output() override{
        std::stringstream ss;
        auto notag=TagTable::create_tag_G();
        auto endtag=TagTable::create_tag_G();
        ss<<"if("<<!<<cond->name<<") goto "<<notag<<";"<<std::endl;
        ss<<ok->output()<<std::endl;
        ss<<"goto "<<endtag<<";"<<std::endl;
        ss<<notag<<":"<<std::endl;
        ss<<no->output()<<std::endl;
        ss<<endtag<<":"<<std::endl;
        return ss.str();
    }
};

