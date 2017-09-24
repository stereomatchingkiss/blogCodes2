import matplotlib.pyplot as plt
import numpy as np

class plot_2d():
    def __init__(self, x_label = 'epoch', y_label = 'Loss', title = 'Neural net loss', min_x = 0, max_x = 0):
        plt.ion()
        self._figure, self._ax = plt.subplots()
        self._lines, = self._ax.plot([],[], 'o')
        #Autoscale on unknown axis and known lims on the other
        self._ax.set_autoscaley_on(True)
        if max_x > min_x and min_x >= 0:
            self._ax.set_xlim(min_x, max_x)
        
        self._ax.grid(True)
        self._ax.set_xlabel(x_label)
        self._ax.set_ylabel(y_label)
        self._ax.set_title(title)

    def update(self, xdata, ydata):
        #Update data (with the new _and_ the old points)
        self._lines.set_xdata(np.append(self._lines.get_xdata(), xdata))
        self._lines.set_ydata(np.append(self._lines.get_ydata(), ydata))
        #Need both of these in order to rescale
        self._ax.relim()
        self._ax.autoscale_view()
        #We need to draw *and* flush
        self._figure.canvas.draw()
        self._figure.canvas.flush_events()
        
    def savefig(self, save_as):
        self._figure.savefig(save_as)

    #Example
    def __call__(self):        
        import time        
        for x in np.arange(0,10,0.5):
            xdata = x
            ydata = np.exp(-x**2)+10*np.exp(-(x-7)**2)
            self.update(xdata, ydata)
            time.sleep(1)
        return xdata, ydata

#d = plot_2d()
#d()
