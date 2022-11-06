#include "Matlab_OutputArchive.hpp"

namespace SerAr {
    using namespace Archives;

        ///-------------------------------------------------------------------------------------------------
    /// <summary>	Constructor. </summary>
    ///
    /// <remarks>	Neumann, 17.02.2017. </remarks>
    ///
    /// <param name="fpath">  	The filepath to the MATLAB file. </param>
    /// <param name="options">	(Optional) options for controlling the operation. </param>
    ///-------------------------------------------------------------------------------------------------
    Matlab_OutputArchive::Matlab_OutputArchive(const std::filesystem::path &fpath, const MatlabOptions &options)
        : OutputArchive(this), m_filepath(fpath), m_MatlabFile(getMatlabFile(fpath, options)) {};

    ///-------------------------------------------------------------------------------------------------
    /// <summary>	Destructor. </summary>
    ///
    /// <remarks>	Neumann, 17.02.2017. </remarks>
    ///-------------------------------------------------------------------------------------------------
    Matlab_OutputArchive::~Matlab_OutputArchive()
    {
        //Cleanup
        while (!Fields.empty())
        {
            mxDestroyArray(std::get<1>(Fields.top()));
            Fields.pop();
        }
    };

    std::unique_ptr<MATFile, void(*)(MATFile*)> Matlab_OutputArchive::getMatlabFile(const std::filesystem::path &fpath, const MatlabOptions &options) const
        {	
            assert(options != MatlabOptions::read);//, "Cannot have a Matlab_OutputArchive with read-only access!");

            if(!fpath.has_filename())
                throw std::runtime_error{ std::string{"Could not open file due to missing filename: "} + fpath.string() };

            std::unique_ptr<MATFile, void(*)(MATFile*)> pMAT(
                matOpen(fpath.string().c_str(), MatlabHelper::getMatlabMode(options)),
                [](MATFile* mat) {matClose(mat);} );

            if (pMAT == nullptr)
                throw std::runtime_error{ std::string{ "Could not open file: " } +fpath.string() };

            return std::move(pMAT);
        }

    void Matlab_OutputArchive::finishMATLABArray()
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
        template void Matlab_OutputArchive::save< type & >(const NamedValue< type &> &); \
        template void Matlab_OutputArchive::save< const type & >(const NamedValue< const type &> &); \
        template void Matlab_OutputArchive::save< type >(const NamedValue< type > &); \
        template void Matlab_OutputArchive::save< const type >(const NamedValue< const type > &); 
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
    MATLAB_ARCHIVE_SAVE(std::filesystem::path)
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
    MATLAB_ARCHIVE_SAVE(std::vector<std::filesystem::path>)
    MATLAB_ARCHIVE_SAVE(std::optional<bool>)
    MATLAB_ARCHIVE_SAVE(std::optional<short>)
    MATLAB_ARCHIVE_SAVE(std::optional<unsigned short>)
    MATLAB_ARCHIVE_SAVE(std::optional<int>)
    MATLAB_ARCHIVE_SAVE(std::optional<unsigned int>)
    //MATLAB_ARCHIVE_SAVE(std::optional<long>)
    //MATLAB_ARCHIVE_SAVE(std::optional<unsigned long>)
    MATLAB_ARCHIVE_SAVE(std::optional<long long>)
    MATLAB_ARCHIVE_SAVE(std::optional<unsigned long long>)
    MATLAB_ARCHIVE_SAVE(std::optional<double>)
    MATLAB_ARCHIVE_SAVE(std::optional<float>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::string>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::filesystem::path>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<short>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<unsigned short>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<int>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<unsigned int>>)
    //MATLAB_ARCHIVE_SAVE(std::optional<std::vector<long>>)
    //MATLAB_ARCHIVE_SAVE(std::optional<std::vector<unsigned long>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<long long>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<unsigned long long>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<double>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<float>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<std::string>>)
    MATLAB_ARCHIVE_SAVE(std::optional<std::vector<std::filesystem::path>>)
    #undef MATLAB_ARCHIVE_SAVE
}
