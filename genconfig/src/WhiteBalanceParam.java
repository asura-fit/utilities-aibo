import java.lang.*;

public class WhiteBalanceParam extends Parameter {
    public static final String LABEL = "WhiteBalance";
    public static final int FL = 0;
    public static final int INDOOR = 1;
    public static final int OUTDOOR = 2;
    private int fValue;
    
    public WhiteBalanceParam(String vals[]) throws Exception {
        // error check
        if(!vals[0].equals(LABEL))
            throw new Exception();
        fValue = parseValue(vals[1]);
    }
    
    protected static int parseValue(String val) throws Exception {
        if(val.equals("FL"))
            return WhiteBalanceParam.FL;
        else if(val.equals("INDOOR"))
            return WhiteBalanceParam.INDOOR;
        else if(val.equals("OUTDOOR"))
            return WhiteBalanceParam.OUTDOOR;
        throw new Exception();
    }
}
