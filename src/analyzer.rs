use clap::ValueEnum;
use crate::Args;

#[derive(Debug, Clone, ValueEnum)]
pub enum WindowFunction {
    Rectangle,
    Gauss,
    VonHann,
    Triangle,
    Blackman
}

#[derive(Debug, Clone)]
pub struct AnalyzerFactory {
    file: String,
    outfile: String,
    samplerate: usize,
    freq_range: Option<(u64, u64)>,
    padding: usize,
    window: WindowFunction,
    channel: Option<usize>
}

impl From<Args> for AnalyzerFactory {
    fn from(args: Args) -> AnalyzerFactory {
        AnalyzerFactory {
            file: args.file.clone(),
            outfile: args.outfile.unwrap_or(format!("{}.json", args.file)),
            samplerate: args.samplerate,
            freq_range: args.freq_range,
            padding: args.padding.unwrap_or(0),
            window: args.window.unwrap_or(WindowFunction::VonHann),
            channel: args.channel
        }
    }
}