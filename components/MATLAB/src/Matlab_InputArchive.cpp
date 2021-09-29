#include "Matlab_InputArchive.hpp"

namespace SerAr 
{

    Matlab_InputArchive::Matlab_InputArchive(const std::filesystem::path &fpath, const MatlabOptions &options)
        : InputArchive(this), m_MatlabFile(getMatlabFile(fpath, options))  {}
    Matlab_InputArchive::~Matlab_InputArchive() 
    {
        //Cleanup
        while (!mFields.empty())
        {
            if (mFields.size() == 1)
                mxDestroyArray(std::get<1>(mFields.top()));

            mFields.pop();
        }
    }

    void Matlab_InputArchive::checkCurrentField()
    {
        if (mFields.empty()) // We are at File level!
            return;
        else
        {
            const auto& top = mFields.top();
            const auto& arr = std::get<1>(top);
            if (mxIsStruct(arr))
                return;
            else
            {
                const auto& name = std::get<0>(top);
                throw std::runtime_error{ std::string{ "Unable to nest current field. Field is not a struct. Fieldname: " } +name };
            }
        }
    };

    void Matlab_InputArchive::releaseField() noexcept
    {
        assert(!mFields.empty());

        const auto top = mFields.top();
        const auto arr = std::get<1>(top);

        //Seems like we only need to destroy the last array within the stack. 
        //Trying to delete all other arrays or only the structs gave an access violation exception from MATLAB! 
        if(mFields.size()==1) 
            mxDestroyArray(arr);

        mFields.pop();
    };

    std::optional<mxArray*> Matlab_InputArchive::loadNextField(const std::string &str, bool optional = false)
    {
        mxArray * nextarr = nullptr;

        if (mFields.empty())
        {
            nextarr = matGetVariable(m_MatlabFile.get(),str.c_str());
        }
        else
        {
            const auto& top = mFields.top();
            const auto arr = std::get<1>(top);
            nextarr = mxGetField(arr, 0, str.c_str());
        }
        if (nextarr == nullptr) {
            if(!optional) { throw std::runtime_error{ std::string{ "Could not access field: " } +str }; }
            return std::nullopt;
        }
        mFields.emplace(str, nextarr);
        return std::get<1>(mFields.top());
    };

    std::unique_ptr<MATFile, void(*)(MATFile*)> Matlab_InputArchive::getMatlabFile(const std::filesystem::path &fpath, const MatlabOptions &options) const
    {
        assert(options == MatlabOptions::read || options == MatlabOptions::update);// , "Cannot have a Matlab_InputArchive with write-only access!");

        if (!fpath.has_filename())
            throw std::runtime_error{ std::string{ "Could not open file due to missing filename: " } +fpath.string() };

        std::unique_ptr<MATFile, void(*)(MATFile*)> pMat(
            matOpen(fpath.string().c_str(), MatlabHelper::getMatlabMode(options)),
            [](MATFile* mat) {matClose(mat);} );

        if (pMat == nullptr)
            throw std::runtime_error{ std::string{ "Could not open file: " } +fpath.string() };

        return std::move(pMat);
    };

    auto Matlab_InputArchive::list(const Archives::NamedValue<decltype(nullptr)>& value) -> std::map<std::string,std::string> {
        return list(value.getName());
    }

    auto Matlab_InputArchive::list(const std::string& str) -> std::map<std::string,std::string> {
        std::map<std::string,std::string> ret;
        if(!mFields.empty()) {
            const auto& current_top = mFields.top();
            const auto arr = mxGetField(std::get<1>(current_top),0,str.c_str());
            const auto num = mxGetNumberOfFields(arr);
            for(int i = 0; i < num; i++ )
            {
                const auto name_cstr = mxGetFieldNameByNumber(arr,i);
                std::string value{};
                getValue(value,mxGetField(arr,0,name_cstr));
                ret.emplace(name_cstr,std::move(value));
                mxFree((void *)name_cstr);
            }
        } else {
           int num;
           const char ** field_names = (const char **)matGetDir(m_MatlabFile.get(), &num);
           for(int i = 0; i < num; i++ )
           {
               auto array = matGetVariable(m_MatlabFile.get(), field_names[0]);
               std::string value{};
               getValue(value,array);
               mxFree(array);
               ret.emplace(field_names[0],value);
           }
           mxFree(field_names);
        }
        return ret;
    };

    #define MATLAB_ARCHIVE_LOAD(type) \
        template void Matlab_InputArchive::load< type &>(NamedValue< type &> &); \
        template void Matlab_InputArchive::load< type >(NamedValue< type > &);
    MATLAB_ARCHIVE_LOAD(bool)
    MATLAB_ARCHIVE_LOAD(short)
    MATLAB_ARCHIVE_LOAD(unsigned short)
    MATLAB_ARCHIVE_LOAD(int)
    MATLAB_ARCHIVE_LOAD(unsigned int)
    //MATLAB_ARCHIVE_LOAD(long)
    //MATLAB_ARCHIVE_LOAD(unsigned long)
    MATLAB_ARCHIVE_LOAD(long long)
    MATLAB_ARCHIVE_LOAD(unsigned long long)
    MATLAB_ARCHIVE_LOAD(double)
    MATLAB_ARCHIVE_LOAD(float)
    MATLAB_ARCHIVE_LOAD(std::string)
    MATLAB_ARCHIVE_LOAD(std::filesystem::path)
    MATLAB_ARCHIVE_LOAD(std::vector<short>)
    MATLAB_ARCHIVE_LOAD(std::vector<unsigned short>)
    MATLAB_ARCHIVE_LOAD(std::vector<int>)
    MATLAB_ARCHIVE_LOAD(std::vector<unsigned int>)
    //MATLAB_ARCHIVE_LOAD(std::vector<long>)
    //MATLAB_ARCHIVE_LOAD(std::vector<unsigned long>)
    MATLAB_ARCHIVE_LOAD(std::vector<long long>)
    MATLAB_ARCHIVE_LOAD(std::vector<unsigned long long>)
    MATLAB_ARCHIVE_LOAD(std::vector<double>)
    MATLAB_ARCHIVE_LOAD(std::vector<float>)
    //MATLAB_ARCHIVE_LOAD(std::vector<std::string>)
    //MATLAB_ARCHIVE_LOAD(std::vector<std::filesystem::path>)
    MATLAB_ARCHIVE_LOAD(std::optional<bool>)
    MATLAB_ARCHIVE_LOAD(std::optional<short>)
    MATLAB_ARCHIVE_LOAD(std::optional<unsigned short>)
    MATLAB_ARCHIVE_LOAD(std::optional<int>)
    MATLAB_ARCHIVE_LOAD(std::optional<unsigned int>)
    //MATLAB_ARCHIVE_LOAD(std::optional<long>)
    //MATLAB_ARCHIVE_LOAD(std::optional<unsigned long>)
    MATLAB_ARCHIVE_LOAD(std::optional<long long>)
    MATLAB_ARCHIVE_LOAD(std::optional<unsigned long long>)
    MATLAB_ARCHIVE_LOAD(std::optional<double>)
    MATLAB_ARCHIVE_LOAD(std::optional<float>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::string>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::filesystem::path>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<short>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<unsigned short>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<int>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<unsigned int>>)
    //MATLAB_ARCHIVE_LOAD(std::optional<std::vector<long>>)
    //MATLAB_ARCHIVE_LOAD(std::optional<std::vector<unsigned long>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<long long>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<unsigned long long>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<double>>)
    MATLAB_ARCHIVE_LOAD(std::optional<std::vector<float>>)
    //MATLAB_ARCHIVE_LOAD(std::optional<std::vector<std::string>>)
    //MATLAB_ARCHIVE_LOAD(std::optional<std::vector<std::filesystem::path>>)
    #undef MATLAB_ARCHIVE_LOAD
}
