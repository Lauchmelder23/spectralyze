mod tuple_parser;
mod analyzer;
use analyzer::WindowFunction;
use clap::{Parser, ValueEnum};

use crate::analyzer::AnalyzerFactory;

#[derive(Parser, Debug)]
#[command(name = "spectralyze")]
#[command(author = "Robert A.")]
#[command(version = "2.0")]
#[command(about = "Performs STFT on audio files", long_about = None)]
pub struct Args {
    /// Audio file to operate on
    file: String,

    /// Suppress output
    #[arg(short = 'q', long = "quiet")]
    quiet: bool,

    /// Output file
    /// 
    /// By default, spectralyze will create the output
    /// file in the current working directory. It will have
    /// the same name as the input file, with a different extension.
    #[arg(short = 'o', long = None)]
    outfile: Option<String>,

    /// Sampling rate of the analysis (in Hz)
    /// 
    /// Specifies the frequency of analysis sample points.
    /// E.g. a value of 100 would perform one FFT every 10ms
    #[arg(short = 's', long = None)]
    samplerate: usize,

    /// The desired range of the spectrum
    /// 
    /// The output range of the frequency spectrum. By default
    /// every resolvable frequency is written to file.
    #[arg(short = 'f', long = "freq", value_parser = tuple_parser::TupleParser)]
    freq_range: Option<(u64, u64)>,

    /// Zero-padding
    /// 
    /// Specifies the level of zero-padding.
    #[arg(short = 'p', long = "padding")]
    padding: Option<usize>,

    /// Desired window function
    /// 
    /// The window function is used to splice the audio signal.
    /// By default, the Von Hann window is used
    #[arg(short = 'w', long = "window", value_enum)]
    window: Option<WindowFunction>,

    /// Analyze only specified channel
    #[arg(short = 'm', long = "mono")]
    channel: Option<usize>
}

fn main() {
    let args = Args::parse();
    let factory = AnalyzerFactory::from(args);

}
