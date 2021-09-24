///-------------------------------------------------------------------------------------------------
// file:	Matlab_Archive.cpp
//
// summary:	Implements the matlab archive class
///-------------------------------------------------------------------------------------------------

#include "Matlab_Archive.h"

#include <memory>

namespace Archives 
{

    MatlabInputArchive::MatlabInputArchive(const std::filesystem::path &fpath, const MatlabOptions &options)
        : InputArchive(this), m_MatlabFile(getMatlabFile(fpath, options))  {}
    MatlabInputArchive::~MatlabInputArchive() 
    {
        //Cleanup
        while (!mFields.empty())
        {
            if (mFields.size() == 1)
                mxDestroyArray(std::get<1>(mFields.top()));

            mFields.pop();
        }
    }

    std::unique_ptr<MATFile, void(*)(MATFile*)> MatlabInputArchive::getMatlabFile(const std::filesystem::path &fpath, const MatlabOptions &options) const
    {
        assert(options == MatlabOptions::read || options == MatlabOptions::update);// , "Cannot have a MatlabInputArchive with write-only access!");

        if (!fpath.has_filename())
            throw std::runtime_error{ std::string{ "Could not open file due to missing filename: " } +fpath.string() };

        std::unique_ptr<MATFile, void(*)(MATFile*)> pMat(
            matOpen(fpath.string().c_str(), MatlabHelper::getMatlabMode(options)),
            [](MATFile* mat) {matClose(mat);} );

        if (pMat == nullptr)
            throw std::runtime_error{ std::string{ "Could not open file: " } +fpath.string() };

        return std::move(pMat);
    };

    auto MatlabInputArchive::list(const Archives::NamedValue<decltype(nullptr)>& value) -> std::map<std::string,std::string> {
        return list(value.getName());
    }

    auto MatlabInputArchive::list(const std::string& str) -> std::map<std::string,std::string> {
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
    ///-------------------------------------------------------------------------------------------------
    /// <summary>	Constructor. </summary>
    ///
    /// <remarks>	Neumann, 17.02.2017. </remarks>
    ///
    /// <param name="fpath">  	The filepath to the MATLAB file. </param>
    /// <param name="options">	(Optional) options for controlling the operation. </param>
    ///-------------------------------------------------------------------------------------------------
    MatlabOutputArchive::MatlabOutputArchive(const std::filesystem::path &fpath, const MatlabOptions &options)
        : OutputArchive(this), m_filepath(fpath), m_options(options), m_MatlabFile(getMatlabFile(fpath, options)) {};

    ///-------------------------------------------------------------------------------------------------
    /// <summary>	Destructor. </summary>
    ///
    /// <remarks>	Neumann, 17.02.2017. </remarks>
    ///-------------------------------------------------------------------------------------------------
    MatlabOutputArchive::~MatlabOutputArchive()
    {
        //Cleanup
        while (!Fields.empty())
        {
            mxDestroyArray(std::get<1>(Fields.top()));
            Fields.pop();
        }
    };

    std::unique_ptr<MATFile, void(*)(MATFile*)> MatlabOutputArchive::getMatlabFile(const std::filesystem::path &fpath, const MatlabOptions &options) const
        {	
            assert(options != MatlabOptions::read);//, "Cannot have a MatlabOutputArchive with read-only access!");

            if(!fpath.has_filename())
                throw std::runtime_error{ std::string{"Could not open file due to missing filename: "} + fpath.string() };

            std::unique_ptr<MATFile, void(*)(MATFile*)> pMAT(
                matOpen(fpath.string().c_str(), MatlabHelper::getMatlabMode(options)),
                [](MATFile* mat) {matClose(mat);} );

            if (pMAT == nullptr)
                throw std::runtime_error{ std::string{ "Could not open file: " } +fpath.string() };

            return std::move(pMAT);
        }

    void MatlabOutputArchive::finishMATLABArray()
    {
        if (Fields.empty())
            return;

        //assert(!Fields.empty());// , "Trying to pop more mxArrays from the stack than had been pushed"); //Programming error!

        auto TopField = Fields.top(); //Thats the field we have to add! (Child mxArray)
        Fields.pop(); //Remove it

        if (Fields.empty()) //at the bottom lvl; write array to mat
        {
            int status = matPutVariable(m_MatlabFile.get(), std::get<0>(TopField).c_str(), std::get<1>(TopField)); // This throws an expection and catches it itself!
            if (status != 0)
                throw std::runtime_error{ "Unable to write Array to MATLAB file! (Out of Memory?)" };
            mxDestroyArray(std::get<1>(TopField)); //release array and automaticlly destroys all arrays which have been added into the array!
        }
        else // we are recursive
        {
            auto& BottomField = Fields.top();
            auto& arr = std::get<1>(BottomField); //Parent mxArray
    
            if (mxIsStruct(arr))
            {
                auto index = mxAddField(arr, std::get<0>(TopField).c_str());

                if (index == -1)
                    throw std::runtime_error{ "Could not add Field to MATLAB struct! (Out of Memory?)" };

                mxSetFieldByNumber(arr, 0, index, std::get<1>(TopField)); // Add Child to parent
                                                                          //no destruction of the TopField array here!
            }
            else if (mxIsCell(arr))
            {
                std::cout << "MATLAB case not handled. Debug me or add correct case" << std::endl;
                //TODO: Implement mxArray insert for cell arrays;
                assert(false);// , "Case (Cell) not handeled by Archive currently!");
            }
            else if (mxIsNumeric(arr))
            {
                std::cout << "MATLAB case not handled. Debug me or add correct case" << std::endl;
                //TODO: Implement mxArray insert for numeric arrays;
                assert(false);// , "Case (Numeric) not handeled by Archive currently!");
            }
            else
            {
                std::cout << "Unknown MATLAB case. Debug me or add correct case" << std::endl;
                assert(true);// , "Case (Unknown) not handeled by Archive!");
            }
        }
    }


    #define MATLAB_ARCHIVE_SAVE(type) \
        template void MatlabOutputArchive::save< type & >(const NamedValue< type &> &); \
        template void MatlabOutputArchive::save< const type & >(const NamedValue< const type &> &); \
        template void MatlabOutputArchive::save< type >(const NamedValue< type > &); \
        template void MatlabOutputArchive::save< const type >(const NamedValue< const type > &); 
    MATLAB_ARCHIVE_SAVE(bool)
    MATLAB_ARCHIVE_SAVE(short)
    MATLAB_ARCHIVE_SAVE(unsigned short)
    MATLAB_ARCHIVE_SAVE(int)
    MATLAB_ARCHIVE_SAVE(unsigned int)
    //MATLAB_ARCHIVE_SAVE(long)
    //MATLAB_ARCHIVE_SAVE(unsigned long)
    MATLAB_ARCHIVE_SAVE(long long)
    MATLAB_ARCHIVE_SAVE(unsigned long long)
    MATLAB_ARCHIVE_SAVE(double)
    MATLAB_ARCHIVE_SAVE(float)
    MATLAB_ARCHIVE_SAVE(std::string)
    MATLAB_ARCHIVE_SAVE(std::vector<short>)
    MATLAB_ARCHIVE_SAVE(std::vector<unsigned short>)
    MATLAB_ARCHIVE_SAVE(std::vector<int>)
    MATLAB_ARCHIVE_SAVE(std::vector<unsigned int>)
    //MATLAB_ARCHIVE_SAVE(std::vector<long>)
    //MATLAB_ARCHIVE_SAVE(std::vector<unsigned long>)
    MATLAB_ARCHIVE_SAVE(std::vector<long long>)
    MATLAB_ARCHIVE_SAVE(std::vector<unsigned long long>)
    MATLAB_ARCHIVE_SAVE(std::vector<double>)
    MATLAB_ARCHIVE_SAVE(std::vector<float>)
    MATLAB_ARCHIVE_SAVE(std::vector<std::string>)
    #undef MATLAB_ARCHIVE_SAVE
    #define MATLAB_ARCHIVE_LOAD(type) \
        template void MatlabInputArchive::load< type &>(NamedValue< type &> &); \
        template void MatlabInputArchive::load< type >(NamedValue< type > &);
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
    #undef MATLAB_ARCHIVE_LOAD
    // template void MatlabOutputArchive::save<bool>(const Archives::NamedValue<bool>& value);
    // template void MatlabOutputArchive::save<short>(const Archives::NamedValue<short>& value);
    // template void MatlabOutputArchive::save<int>(const Archives::NamedValue<int>& value);
    // //template void MatlabOutputArchive::save<long>(const Archives::NamedValue<long>& value);
    // template void MatlabOutputArchive::save<long long>(const Archives::NamedValue<long long>& value);
    // template void MatlabOutputArchive::save<unsigned int>(const Archives::NamedValue<unsigned int>& value);
    // //template void MatlabOutputArchive::save<unsigned long>(const Archives::NamedValue<unsigned long>& value);
    // template void MatlabOutputArchive::save<unsigned long long>(const Archives::NamedValue<unsigned long long>& value);
    // template void MatlabOutputArchive::save<float>(const Archives::NamedValue<float>& value);
    // template void MatlabOutputArchive::save<double>(const Archives::NamedValue<double>& value);
    // template void MatlabOutputArchive::save<long double>(const Archives::NamedValue<long double>& value);
    // template void MatlabOutputArchive::save<std::string>(const Archives::NamedValue<std::string>& value);
    // template void MatlabOutputArchive::save<bool&>(const Archives::NamedValue<bool&>& value);
    // template void MatlabOutputArchive::save<short&>(const Archives::NamedValue<short&>& value);
    // template void MatlabOutputArchive::save<int&>(const Archives::NamedValue<int&>& value);
    // //template void MatlabOutputArchive::save<long&>(const Archives::NamedValue<long&>& value);
    // template void MatlabOutputArchive::save<long long&>(const Archives::NamedValue<long long&>& value);
    // template void MatlabOutputArchive::save<unsigned int&>(const Archives::NamedValue<unsigned int&>& value);
    // //template void MatlabOutputArchive::save<unsigned long&>(const Archives::NamedValue<unsigned long&>& value);
    // template void MatlabOutputArchive::save<unsigned long long&>(const Archives::NamedValue<unsigned long long&>& value);
    // template void MatlabOutputArchive::save<float&>(const Archives::NamedValue<float&>& value);
    // template void MatlabOutputArchive::save<double&>(const Archives::NamedValue<double&>& value);
    // template void MatlabOutputArchive::save<long double&>(const Archives::NamedValue<long double&>& value);
    // template void MatlabOutputArchive::save<std::string&>(const Archives::NamedValue<std::string&>& value);
}
