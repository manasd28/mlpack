/**
 * @file perceptron_test.cpp
 * @author Manish Kumar
 *
 * Test mlpackMain() of perceptron_main.cpp.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#define BINDING_TYPE BINDING_TYPE_TEST

#include <mlpack/core.hpp>
static const std::string testName = "Perceptron";

#include <mlpack/core/util/mlpack_main.hpp>
#include <mlpack/methods/perceptron/perceptron_main.cpp>
#include "test_helper.hpp"

#include <boost/test/unit_test.hpp>
#include "../test_tools.hpp"

using namespace mlpack;

struct PerceptronTestFixture
{
 public:
  PerceptronTestFixture()
  {
    // Cache in the options for this program.
    CLI::RestoreSettings(testName);
  }

  ~PerceptronTestFixture()
  {
    // Clear the settings.
    CLI::ClearSettings();
  }
};

// reset the parameters
void resetSettings() {
  CLI::ClearSettings();
    CLI::RestoreSettings(testName);
}

BOOST_FIXTURE_TEST_SUITE(PerceptronMainTest,
                         PerceptronTestFixture);

/**
 * Ensure that we get desired dimensions when both training
 * data and labels are passed.
 */
BOOST_AUTO_TEST_CASE(PerceptronOutputDimensionTest)
{
  arma::mat inputData;
  if (!data::Load("trainSet.csv", inputData))
    BOOST_FAIL("Cannot load train dataset trainSet.csv!");

  // Get the labels out.
  arma::Row<size_t> labels(inputData.n_cols);
  for (size_t i = 0; i < inputData.n_cols; ++i)
    labels[i] = inputData(inputData.n_rows - 1, i);

  // Delete the last row containing labels from input dataset.
  inputData.shed_row(inputData.n_rows - 1);

  arma::mat testData;
  if (!data::Load("testSet.csv", testData))
    BOOST_FAIL("Cannot load test dataset testSet.csv!");

  // Delete the last row containing labels from test dataset.
  testData.shed_row(testData.n_rows - 1);

  size_t testSize = testData.n_cols;

  // Input training data.
  SetInputParam("training", std::move(inputData));
  SetInputParam("labels", std::move(labels));

  // Input test data.
  SetInputParam("test", std::move(testData));

  mlpackMain();

  // Check that number of output points are equal to number of input points.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_cols,
                      testSize);

  // Check output have only single row.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_rows, 1);
}

/**
 * Check that last row of input file is used as labels
 * when labels are not passed specifically and results
 * are same from both label and labeless models.
 */
BOOST_AUTO_TEST_CASE(PerceptronLabelsLessDimensionTest)
{
  // Train perceptron without providing labels.
  arma::mat inputData;
  if (!data::Load("trainSet.csv", inputData))
    BOOST_FAIL("Cannot load train dataset trainSet.csv!");

  // Get the labels out.
  arma::Row<size_t> labels(inputData.n_cols);
  for (size_t i = 0; i < inputData.n_cols; ++i)
    labels[i] = inputData(inputData.n_rows - 1, i);

  arma::mat testData;
  if (!data::Load("testSet.csv", testData))
    BOOST_FAIL("Cannot load test dataset testSet.csv!");

  // Delete the last row containing labels from test dataset.
  testData.shed_row(testData.n_rows - 1);

  size_t testSize = testData.n_cols;

  // Input training data.
  SetInputParam("training", inputData);

  // Input test data.
  SetInputParam("test", testData);

  mlpackMain();

  // Check that number of output points are equal to number of input points.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_cols,
                      testSize);

  // Check output have only single row.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_rows, 1);

  // Reset data passed.
  CLI::GetSingleton().Parameters()["training"].wasPassed = false;
  CLI::GetSingleton().Parameters()["test"].wasPassed = false;

  inputData.shed_row(inputData.n_rows - 1);

  // Store outputs.
  arma::Row<size_t> output;
  output = std::move(CLI::GetParam<arma::Row<size_t>>("output"));

  // Now train pereptron with labels provided.

  // Input training data.
  SetInputParam("training", std::move(inputData));
  SetInputParam("test", std::move(testData));
  // Pass Labels.
  SetInputParam("labels", std::move(labels));

  mlpackMain();

  // Check that number of output points are equal to number of input points.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_cols,
                      testSize);

  // Check output have only single row.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_rows, 1);

  // Check that initial output and final output matrix
  // from two models are same.
  CheckMatrices(output, CLI::GetParam<arma::Row<size_t>>("output"));
}

/**
 * Ensure that saved model can be used again.
 */
BOOST_AUTO_TEST_CASE(PerceptronModelReuseTest)
{
  arma::mat inputData;
  if (!data::Load("trainSet.csv", inputData))
    BOOST_FAIL("Cannot load train dataset trainSet.csv!");

  arma::mat testData;
  if (!data::Load("testSet.csv", testData))
    BOOST_FAIL("Cannot load test dataset testSet.csv!");

  // Delete the last row containing labels from test dataset.
  testData.shed_row(testData.n_rows - 1);

  size_t testSize = testData.n_cols;

  // Input training data.
  SetInputParam("training", std::move(inputData));

  // Input test data.
  SetInputParam("test", testData);

  mlpackMain();

  arma::Row<size_t> output;
  output = std::move(CLI::GetParam<arma::Row<size_t>>("output"));

  // Reset passed parameters.
  CLI::GetSingleton().Parameters()["training"].wasPassed = false;
  CLI::GetSingleton().Parameters()["test"].wasPassed = false;

  // Input trained model.
  SetInputParam("test", std::move(testData));
  SetInputParam("input_model",
                std::move(CLI::GetParam<PerceptronModel>("output_model")));

  mlpackMain();

  // Check that number of output points are equal to number of input points.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_cols,
                      testSize);

  // Check output have only single row.
  BOOST_REQUIRE_EQUAL(CLI::GetParam<arma::Row<size_t>>("output").n_rows, 1);

  // Check that initial output and final output matrix
  // using saved model are same.
  CheckMatrices(output, CLI::GetParam<arma::Row<size_t>>("output"));
}

/**
 * Ensure that max_iterations is always non-negative.
 */
BOOST_AUTO_TEST_CASE(PerceptronMaxItrTest)
{
  arma::mat inputData;
  if (!data::Load("trainSet.csv", inputData))
    BOOST_FAIL("Cannot load train dataset trainSet.csv!");

  // Input training data.
  SetInputParam("training", std::move(inputData));
  SetInputParam("max_iterations", (int) -1);

  Log::Fatal.ignoreInput = true;
  BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
  Log::Fatal.ignoreInput = false;
}

/**
  * Ensuring that re-training of an existing model with different of classes is checked
 **/
BOOST_AUTO_TEST_CASE(PerceptronReTrainWithWrongClasses)
{
  arma::mat trainX1; 
  arma::Row<size_t> labelsX1;

  // loading a train data set with 3 classes
  if(!data::Load("vc2.csv",trainX1)) {
    BOOST_FAIL("Could not load the train data (vc2.csv)");
  }

  // loading the corresponding labels to the dataset
  if(!data::Load("vc2_labels.txt", labelsX1)) {
    BOOST_FAIL("Could not load the train data (vc2_labels.csv)");
  }

  SetInputParam("training", std::move(trainX1)); // train data
  SetInputParam("labels", std::move(labelsX1)); // labels for the train data

  //training model using first training dataset
  mlpackMain();

  // get the output model obtained after training
  PerceptronModel model = std::move(CLI::GetParam<PerceptronModel>("output_model"));

  resetSettings();

  arma::mat trainX2;

  // loading a train dataset with 5 classes
  if(!data::Load("train_data_5_classes.csv",trainX2)) {
    BOOST_FAIL("Could not load the train data train_data_5_classes.csv");
  }

  SetInputParam("training",std::move(trainX2)); //last column of trainX2 contains the class labels
  SetInputParam("input_model",std::move(model));

  // re-training an existing model of 3 classes with training data of 5 classes. Should give runtime error
  Log::Fatal.ignoreInput=true;
  BOOST_REQUIRE_THROW(mlpackMain(),std::runtime_error);
  Log::Fatal.ignoreInput=false;
}

/**
  * Checking for dimensionality of the test data set
 **/
BOOST_AUTO_TEST_CASE(PerceptronWrongDimOfTestData) 
{
  constexpr int N = 10;
  constexpr int D = 4;
  constexpr int M = 20;

  arma::mat trainX = arma::randu<arma::mat>(D,N);
  arma::Row<size_t> trainY;

  trainY << 0 << 1 << 0 << 1 << 1 << 1 << 0 << 1 << 0 << 0 << endr; // 10 responses

  arma::mat testX = arma::randu<arma::mat>(D-3,M);  // test data with wrong dimensionality

  SetInputParam("training", std::move(trainX));
  // SetInputParam("labels", std::move(trainY));
  SetInputParam("test", std::move(testX));

  // test data set with wrong dimensionality. Should give runtime error
  Log::Fatal.ignoreInput = true;
  BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
  Log::Fatal.ignoreInput = false;

}

/**
  * Ensuring that the response size is checked
 **/
BOOST_AUTO_TEST_CASE(PerceptronWrongResponseSizeTest) 
{
  constexpr int D = 2;
  constexpr int N = 10;
  
  arma::mat trainX = arma::randu<arma::mat>(D,N);
  arma::Row<size_t> trainY; // response vector with wrong size

  trainY << 0 << 0 << 1 << 0 << 1 << 1 << 1 << 0 << 1 << 0 << 1 << 0 << endr; // 8 responses

  SetInputParam("training", std::move(trainX));
  SetInputParam("labels", std::move(trainY));

  // labels for training data have wrong size. Should give runtime error 
  Log::Fatal.ignoreInput = true;
  BOOST_REQUIRE_THROW(mlpackMain(),std::runtime_error);
  Log::Fatal.ignoreInput = false;
}

/**
 * Ensuring that absence of responses is checked.
 */
BOOST_AUTO_TEST_CASE(PerceptronNoResponsesTest)
{
  constexpr int N = 10;
  constexpr int D = 1;

  arma::mat trainX = arma::randu<arma::mat>(D, N);
  SetInputParam("training", std::move(trainX));

  // No labels for training data. Should give runtime error
  Log::Fatal.ignoreInput = true;
  BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
  Log::Fatal.ignoreInput = false;
}


/**
 * Ensuring that absence of training data is checked.
 */
BOOST_AUTO_TEST_CASE(PerceptronNoTrainingDataTest)
{
  arma::Row<size_t> trainY;
  trainY << 1 << 1 << 0 << 1 << 0 << 0 <<endr;

  SetInputParam("labels", std::move(trainY));

  // No training data. Should give runtime error.
  Log::Fatal.ignoreInput = true;
  BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
  Log::Fatal.ignoreInput = false;
}


/**
 * Ensuring that test data dimensionality is checked when model is loaded.
 */
BOOST_AUTO_TEST_CASE(PerceptronWrongDimOfTestData2)
{
  constexpr int N = 10;
  constexpr int D = 3;
  constexpr int M = 15;

  arma::mat trainX = arma::randu<arma::mat>(D, N);
  arma::Row<size_t> trainY;

  trainY << 0 << 1 << 0 << 1 << 1 << 1 << 0 << 1 << 0 << 0 << endr; // 10 responses

  SetInputParam("training", std::move(trainX));
  SetInputParam("labels", std::move(trainY));

  // training the model
  mlpackMain();

  // get the output model obtained after the training
  PerceptronModel model = std::move(CLI::GetParam<PerceptronModel>("output_model"));

  resetSettings();

  arma::mat testX = arma::randu<arma::mat>(D - 1, M); // test data with Wrong dimensionality.
  SetInputParam("input_model", std::move(model));
  SetInputParam("test", std::move(testX));

  // wrong dimensionality of test data. Should give runtime error. 
  Log::Fatal.ignoreInput = true;
  BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
  Log::Fatal.ignoreInput = false;
}

BOOST_AUTO_TEST_SUITE_END();


